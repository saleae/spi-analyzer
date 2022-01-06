
#include "SpiAnalyzer.h"
#include "SpiAnalyzerSettings.h"

#include <AnalyzerChannelData.h>


// enum SpiBubbleType { SpiData, SpiError };

SpiAnalyzer::SpiAnalyzer()
    : Analyzer2(),
      mSettings( new SpiAnalyzerSettings() ),
      mSimulationInitilized( false ),
      mMosi( NULL ),
      mMiso( NULL ),
      mClock( NULL ),
      mEnable( NULL )
{
    SetAnalyzerSettings( mSettings.get() );
    UseFrameV2();
}

SpiAnalyzer::~SpiAnalyzer()
{
    KillThread();
}

void SpiAnalyzer::SetupResults()
{
    mResults.reset( new SpiAnalyzerResults( this, mSettings.get() ) );
    SetAnalyzerResults( mResults.get() );

    if( mSettings->mMosiChannel != UNDEFINED_CHANNEL )
        mResults->AddChannelBubblesWillAppearOn( mSettings->mMosiChannel );
    if( mSettings->mMisoChannel != UNDEFINED_CHANNEL )
        mResults->AddChannelBubblesWillAppearOn( mSettings->mMisoChannel );
}

void SpiAnalyzer::WorkerThread()
{
    Setup();

    AdvanceToActiveEnableEdgeWithCorrectClockPolarity();

    for( ;; )
    {
        GetWord();
        CheckIfThreadShouldExit();
    }
}

void SpiAnalyzer::AdvanceToActiveEnableEdgeWithCorrectClockPolarity()
{
    mResults->CommitPacketAndStartNewPacket();
    mResults->CommitResults();

    AdvanceToActiveEnableEdge();

    for( ;; )
    {
        if( IsInitialClockPolarityCorrect() == true ) // if false, this function moves to the next active enable edge.
        {
            if( mEnable )
            {
                FrameV2 frame_v2_start_of_transaction;
                mResults->AddFrameV2( frame_v2_start_of_transaction, "enable", mCurrentSample, mCurrentSample + 1 );
            }
            break;
        }
    }
}

void SpiAnalyzer::Setup()
{
    bool allow_last_trailing_clock_edge_to_fall_outside_enable = false;
    if( mSettings->mDataValidEdge == AnalyzerEnums::LeadingEdge )
        allow_last_trailing_clock_edge_to_fall_outside_enable = true;

    if( mSettings->mClockInactiveState == BIT_LOW )
    {
        if( mSettings->mDataValidEdge == AnalyzerEnums::LeadingEdge )
            mArrowMarker = AnalyzerResults::UpArrow;
        else
            mArrowMarker = AnalyzerResults::DownArrow;
    }
    else
    {
        if( mSettings->mDataValidEdge == AnalyzerEnums::LeadingEdge )
            mArrowMarker = AnalyzerResults::DownArrow;
        else
            mArrowMarker = AnalyzerResults::UpArrow;
    }


    if( mSettings->mMosiChannel != UNDEFINED_CHANNEL )
        mMosi = GetAnalyzerChannelData( mSettings->mMosiChannel );
    else
        mMosi = NULL;

    if( mSettings->mMisoChannel != UNDEFINED_CHANNEL )
        mMiso = GetAnalyzerChannelData( mSettings->mMisoChannel );
    else
        mMiso = NULL;


    mClock = GetAnalyzerChannelData( mSettings->mClockChannel );

    if( mSettings->mEnableChannel != UNDEFINED_CHANNEL )
        mEnable = GetAnalyzerChannelData( mSettings->mEnableChannel );
    else
        mEnable = NULL;
}

void SpiAnalyzer::AdvanceToActiveEnableEdge()
{
    if( mEnable != NULL )
    {
        if( mEnable->GetBitState() != mSettings->mEnableActiveState )
        {
            mEnable->AdvanceToNextEdge();
        }
        else
        {
            mEnable->AdvanceToNextEdge();
            mEnable->AdvanceToNextEdge();
        }
        mCurrentSample = mEnable->GetSampleNumber();
        mClock->AdvanceToAbsPosition( mCurrentSample );
    }
    else
    {
        mCurrentSample = mClock->GetSampleNumber();
    }
}

bool SpiAnalyzer::IsInitialClockPolarityCorrect()
{
    if( mClock->GetBitState() == mSettings->mClockInactiveState )
        return true;

    mResults->AddMarker( mCurrentSample, AnalyzerResults::ErrorSquare, mSettings->mClockChannel );

    if( mEnable != NULL )
    {
        Frame error_frame;
        error_frame.mStartingSampleInclusive = mCurrentSample;

        mEnable->AdvanceToNextEdge();
        mCurrentSample = mEnable->GetSampleNumber();

        error_frame.mEndingSampleInclusive = mCurrentSample;
        error_frame.mFlags = SPI_ERROR_FLAG | DISPLAY_AS_ERROR_FLAG;
        mResults->AddFrame( error_frame );

        FrameV2 framev2;
        mResults->AddFrameV2( framev2, "error", error_frame.mStartingSampleInclusive, error_frame.mEndingSampleInclusive + 1 );

        mResults->CommitResults();
        ReportProgress( error_frame.mEndingSampleInclusive );

        // move to the next active-going enable edge
        mEnable->AdvanceToNextEdge();
        mCurrentSample = mEnable->GetSampleNumber();
        mClock->AdvanceToAbsPosition( mCurrentSample );

        return false;
    }
    else
    {
        mClock->AdvanceToNextEdge(); // at least start with the clock in the idle state.
        mCurrentSample = mClock->GetSampleNumber();
        return true;
    }
}

bool SpiAnalyzer::WouldAdvancingTheClockToggleEnable( bool add_disable_frame, U64* disable_frame )
{
    if( mEnable == NULL )
        return false;

    auto log_disable_event = [&]( U64 enable_edge ) {
        if( add_disable_frame )
        {
            FrameV2 frame_v2_end_of_transaction;
            mResults->AddFrameV2( frame_v2_end_of_transaction, "disable", enable_edge, enable_edge + 1 );
        }
        else if( disable_frame != nullptr )
        {
            *disable_frame = enable_edge;
        }
    };

    // if the enable is currently active, and there are no more clock transitions in the capture, attempt to capture the final disable event
    if( !mClock->DoMoreTransitionsExistInCurrentData() && mEnable->GetBitState() == mSettings->mEnableActiveState )
    {
        if( mEnable->DoMoreTransitionsExistInCurrentData() )
        {
            U64 next_enable_edge = mEnable->GetSampleOfNextEdge();
            // double check that the clock line actually processed all samples up to the next enable edge.
            // double check is required becase data is getting processed while we're running, it's possible more has already become
            // available.
            if( !mClock->WouldAdvancingToAbsPositionCauseTransition( next_enable_edge ) )
            {
                log_disable_event( next_enable_edge );
                return true;
            }
        }
    }

    U64 next_edge = mClock->GetSampleOfNextEdge();
    bool enable_will_toggle = mEnable->WouldAdvancingToAbsPositionCauseTransition( next_edge );

    if( enable_will_toggle )
    {
        U64 enable_edge = mEnable->GetSampleOfNextEdge();
        log_disable_event( enable_edge );
    }

    if( enable_will_toggle == false )
        return false;
    else
        return true;
}

void SpiAnalyzer::GetWord()
{
    // we're assuming we come into this function with the clock in the idle state;

    const U32 bits_per_transfer = mSettings->mBitsPerTransfer;
    const U32 bytes_per_transfer = ( bits_per_transfer + 7 ) / 8;

    U64 mosi_word = 0;
    mMosiResult.Reset( &mosi_word, mSettings->mShiftOrder, bits_per_transfer );

    U64 miso_word = 0;
    mMisoResult.Reset( &miso_word, mSettings->mShiftOrder, bits_per_transfer );

    U64 first_sample = 0;
    bool need_reset = false;
    U64 disable_event_sample = 0;


    mArrowLocations.clear();
    ReportProgress( mClock->GetSampleNumber() );

    for( U32 i = 0; i < bits_per_transfer; i++ )
    {
        if( i == 0 )
            CheckIfThreadShouldExit();

        // on every single edge, we need to check that enable doesn't toggle.
        // note that we can't just advance the enable line to the next edge, becuase there may not be another edge

        if( WouldAdvancingTheClockToggleEnable( true, nullptr ) == true )
        {
            AdvanceToActiveEnableEdgeWithCorrectClockPolarity(); // ok, we pretty much need to reset everything and return.
            return;
        }

        mClock->AdvanceToNextEdge();
        if( i == 0 )
            first_sample = mClock->GetSampleNumber();

        if( mSettings->mDataValidEdge == AnalyzerEnums::LeadingEdge )
        {
            mCurrentSample = mClock->GetSampleNumber();
            if( mMosi != NULL )
            {
                mMosi->AdvanceToAbsPosition( mCurrentSample );
                mMosiResult.AddBit( mMosi->GetBitState() );
            }
            if( mMiso != NULL )
            {
                mMiso->AdvanceToAbsPosition( mCurrentSample );
                mMisoResult.AddBit( mMiso->GetBitState() );
            }
            mArrowLocations.push_back( mCurrentSample );
        }


        // ok, the trailing edge is messy -- but only on the very last bit.
        // If the trialing edge isn't doesn't represent valid data, we want to allow the enable line to rise before the clock trialing edge
        // -- and still report the frame
        if( ( i == ( bits_per_transfer - 1 ) ) && ( mSettings->mDataValidEdge != AnalyzerEnums::TrailingEdge ) )
        {
            // if this is the last bit, and the trailing edge doesn't represent valid data
            if( WouldAdvancingTheClockToggleEnable( false, &disable_event_sample ) == true )
            {
                // moving to the trailing edge would cause the clock to revert to inactive.  jump out, record the frame, and them move to
                // the next active enable edge
                need_reset = true;
                break;
            }

            // enable isn't going to go inactive, go ahead and advance the clock as usual.  Then we're done, jump out and record the frame.
            mClock->AdvanceToNextEdge();
            break;
        }

        // this isn't the very last bit, etc, so proceed as normal
        if( WouldAdvancingTheClockToggleEnable( true, nullptr ) == true )
        {
            AdvanceToActiveEnableEdgeWithCorrectClockPolarity(); // ok, we pretty much need to reset everything and return.
            return;
        }

        mClock->AdvanceToNextEdge();

        if( mSettings->mDataValidEdge == AnalyzerEnums::TrailingEdge )
        {
            mCurrentSample = mClock->GetSampleNumber();
            if( mMosi != NULL )
            {
                mMosi->AdvanceToAbsPosition( mCurrentSample );
                mMosiResult.AddBit( mMosi->GetBitState() );
            }
            if( mMiso != NULL )
            {
                mMiso->AdvanceToAbsPosition( mCurrentSample );
                mMisoResult.AddBit( mMiso->GetBitState() );
            }
            mArrowLocations.push_back( mCurrentSample );
        }
    }

    // save the results:
    U32 count = mArrowLocations.size();
    for( U32 i = 0; i < count; i++ )
        mResults->AddMarker( mArrowLocations[ i ], mArrowMarker, mSettings->mClockChannel );

    Frame result_frame;
    result_frame.mStartingSampleInclusive = first_sample;
    result_frame.mEndingSampleInclusive = mClock->GetSampleNumber();
    result_frame.mData1 = mosi_word;
    result_frame.mData2 = miso_word;
    result_frame.mFlags = 0;
    mResults->AddFrame( result_frame );

    FrameV2 framev2;

    // Max bits per transfer == 64, max bytes == 8
    U8 mosi_bytearray[ 8 ];
    U8 miso_bytearray[ 8 ];
    for( int i = 0; i < bytes_per_transfer; ++i )
    {
        auto bit_offset = ( bytes_per_transfer - i - 1 ) * 8;
        mosi_bytearray[ i ] = mosi_word >> bit_offset;
        miso_bytearray[ i ] = miso_word >> bit_offset;
    }
    framev2.AddByteArray( "mosi", mosi_bytearray, bytes_per_transfer );
    framev2.AddByteArray( "miso", miso_bytearray, bytes_per_transfer );

    mResults->AddFrameV2( framev2, "result", first_sample, mClock->GetSampleNumber() + 1 );

    mResults->CommitResults();

    if( need_reset == true )
    {
        FrameV2 frame_v2_end_of_transaction;
        mResults->AddFrameV2( frame_v2_end_of_transaction, "disable", disable_event_sample, disable_event_sample + 1 );
        AdvanceToActiveEnableEdgeWithCorrectClockPolarity();
    }
}

bool SpiAnalyzer::NeedsRerun()
{
    return false;
}

U32 SpiAnalyzer::GenerateSimulationData( U64 minimum_sample_index, U32 device_sample_rate,
                                         SimulationChannelDescriptor** simulation_channels )
{
    if( mSimulationInitilized == false )
    {
        mSimulationDataGenerator.Initialize( GetSimulationSampleRate(), mSettings.get() );
        mSimulationInitilized = true;
    }

    return mSimulationDataGenerator.GenerateSimulationData( minimum_sample_index, device_sample_rate, simulation_channels );
}


U32 SpiAnalyzer::GetMinimumSampleRateHz()
{
    return 10000; // we don't have any idea, depends on the SPI rate, etc.; return the lowest rate.
}

const char* SpiAnalyzer::GetAnalyzerName() const
{
    return "SPI";
}

const char* GetAnalyzerName()
{
    return "SPI";
}

Analyzer* CreateAnalyzer()
{
    return new SpiAnalyzer();
}

void DestroyAnalyzer( Analyzer* analyzer )
{
    delete analyzer;
}
