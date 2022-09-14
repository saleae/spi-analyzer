#include "SpiAnalyzerSettings.h"

#include <AnalyzerHelpers.h>
#include <sstream>
#include <cstring>

SpiAnalyzerSettings::SpiAnalyzerSettings()
    : mMosiChannel( UNDEFINED_CHANNEL ),
      mMisoChannel( UNDEFINED_CHANNEL ),
      mClockChannel( UNDEFINED_CHANNEL ),
      mEnableChannel( UNDEFINED_CHANNEL ),
      mShiftOrder( AnalyzerEnums::MsbFirst ),
      mBitsPerTransfer( 8 ),
      mClockInactiveState( BIT_LOW ),
      mDataValidEdge( AnalyzerEnums::LeadingEdge ),
      mEnableActiveState( BIT_LOW )
{
    mMosiChannelInterface.reset( new AnalyzerSettingInterfaceChannel() );
    mMosiChannelInterface->SetTitleAndTooltip( "MOSI", "Master Out, Slave In" );
    mMosiChannelInterface->SetChannel( mMosiChannel );
    mMosiChannelInterface->SetSelectionOfNoneIsAllowed( true );

    mMisoChannelInterface.reset( new AnalyzerSettingInterfaceChannel() );
    mMisoChannelInterface->SetTitleAndTooltip( "MISO", "Master In, Slave Out" );
    mMisoChannelInterface->SetChannel( mMisoChannel );
    mMisoChannelInterface->SetSelectionOfNoneIsAllowed( true );

    mClockChannelInterface.reset( new AnalyzerSettingInterfaceChannel() );
    mClockChannelInterface->SetTitleAndTooltip( "Clock", "Clock (CLK)" );
    mClockChannelInterface->SetChannel( mClockChannel );

    mEnableChannelInterface.reset( new AnalyzerSettingInterfaceChannel() );
    mEnableChannelInterface->SetTitleAndTooltip( "Enable", "Enable (SS, Slave Select)" );
    mEnableChannelInterface->SetChannel( mEnableChannel );
    mEnableChannelInterface->SetSelectionOfNoneIsAllowed( true );

    mShiftOrderInterface.reset( new AnalyzerSettingInterfaceNumberList() );
    mShiftOrderInterface->SetTitleAndTooltip( "Significant Bit", "" );
    mShiftOrderInterface->AddNumber( AnalyzerEnums::MsbFirst, "Most Significant Bit First (Standard)",
                                     "Select if the most significant bit or least significant bit is transmitted first" );
    mShiftOrderInterface->AddNumber( AnalyzerEnums::LsbFirst, "Least Significant Bit First", "" );
    mShiftOrderInterface->SetNumber( mShiftOrder );

    mBitsPerTransferInterface.reset( new AnalyzerSettingInterfaceNumberList() );
    mBitsPerTransferInterface->SetTitleAndTooltip( "Bits per Transfer", "" );
    for( U32 i = 1; i <= 64; i++ )
    {
        std::stringstream ss;

        if( i == 8 )
            ss << "8 Bits per Transfer (Standard)";
        else
            ss << i << " Bits per Transfer";

        mBitsPerTransferInterface->AddNumber( i, ss.str().c_str(), "" );
    }
    mBitsPerTransferInterface->SetNumber( mBitsPerTransfer );

    mClockInactiveStateInterface.reset( new AnalyzerSettingInterfaceNumberList() );
    mClockInactiveStateInterface->SetTitleAndTooltip( "Clock State", "" );
    mClockInactiveStateInterface->AddNumber( BIT_LOW, "Clock is Low when inactive (CPOL = 0)", "CPOL = 0 (Clock Polarity)" );
    mClockInactiveStateInterface->AddNumber( BIT_HIGH, "Clock is High when inactive (CPOL = 1)", "CPOL = 1 (Clock Polarity)" );
    mClockInactiveStateInterface->SetNumber( mClockInactiveState );

    mDataValidEdgeInterface.reset( new AnalyzerSettingInterfaceNumberList() );
    mDataValidEdgeInterface->SetTitleAndTooltip( "Clock Phase", "" );
    mDataValidEdgeInterface->AddNumber( AnalyzerEnums::LeadingEdge, "Data is Valid on Clock Leading Edge (CPHA = 0)",
                                        "CPHA = 0 (Clock Phase)" );
    mDataValidEdgeInterface->AddNumber( AnalyzerEnums::TrailingEdge, "Data is Valid on Clock Trailing Edge (CPHA = 1)",
                                        "CPHA = 1 (Clock Phase)" );
    mDataValidEdgeInterface->SetNumber( mDataValidEdge );

    mEnableActiveStateInterface.reset( new AnalyzerSettingInterfaceNumberList() );
    mEnableActiveStateInterface->SetTitleAndTooltip( "Enable Line", "" );
    mEnableActiveStateInterface->AddNumber( BIT_LOW, "Enable line is Active Low (Standard)", "" );
    mEnableActiveStateInterface->AddNumber( BIT_HIGH, "Enable line is Active High", "" );
    mEnableActiveStateInterface->SetNumber( mEnableActiveState );


    AddInterface( mMosiChannelInterface.get() );
    AddInterface( mMisoChannelInterface.get() );
    AddInterface( mClockChannelInterface.get() );
    AddInterface( mEnableChannelInterface.get() );
    AddInterface( mShiftOrderInterface.get() );
    AddInterface( mBitsPerTransferInterface.get() );
    AddInterface( mClockInactiveStateInterface.get() );
    AddInterface( mDataValidEdgeInterface.get() );
    AddInterface( mEnableActiveStateInterface.get() );


    // AddExportOption( 0, "Export as text/csv file", "text (*.txt);;csv (*.csv)" );
    AddExportOption( 0, "Export as text/csv file" );
    AddExportExtension( 0, "text", "txt" );
    AddExportExtension( 0, "csv", "csv" );

    ClearChannels();
    AddChannel( mMosiChannel, "MOSI", false );
    AddChannel( mMisoChannel, "MISO", false );
    AddChannel( mClockChannel, "CLOCK", false );
    AddChannel( mEnableChannel, "ENABLE", false );
}

SpiAnalyzerSettings::~SpiAnalyzerSettings()
{
}

bool SpiAnalyzerSettings::SetSettingsFromInterfaces()
{
    Channel mosi = mMosiChannelInterface->GetChannel();
    Channel miso = mMisoChannelInterface->GetChannel();
    Channel clock = mClockChannelInterface->GetChannel();
    Channel enable = mEnableChannelInterface->GetChannel();

    std::vector<Channel> channels;
    channels.push_back( mosi );
    channels.push_back( miso );
    channels.push_back( clock );
    channels.push_back( enable );

    if( AnalyzerHelpers::DoChannelsOverlap( &channels[ 0 ], channels.size() ) == true )
    {
        SetErrorText( "Please select different channels for each input." );
        return false;
    }

    if( ( mosi == UNDEFINED_CHANNEL ) && ( miso == UNDEFINED_CHANNEL ) )
    {
        SetErrorText( "Please select at least one input for either MISO or MOSI." );
        return false;
    }

    mMosiChannel = mMosiChannelInterface->GetChannel();
    mMisoChannel = mMisoChannelInterface->GetChannel();
    mClockChannel = mClockChannelInterface->GetChannel();
    mEnableChannel = mEnableChannelInterface->GetChannel();

    mShiftOrder = ( AnalyzerEnums::ShiftOrder )U32( mShiftOrderInterface->GetNumber() );
    mBitsPerTransfer = U32( mBitsPerTransferInterface->GetNumber() );
    mClockInactiveState = ( BitState )U32( mClockInactiveStateInterface->GetNumber() );
    mDataValidEdge = ( AnalyzerEnums::Edge )U32( mDataValidEdgeInterface->GetNumber() );
    mEnableActiveState = ( BitState )U32( mEnableActiveStateInterface->GetNumber() );

    ClearChannels();
    AddChannel( mMosiChannel, "MOSI", mMosiChannel != UNDEFINED_CHANNEL );
    AddChannel( mMisoChannel, "MISO", mMisoChannel != UNDEFINED_CHANNEL );
    AddChannel( mClockChannel, "CLOCK", mClockChannel != UNDEFINED_CHANNEL );
    AddChannel( mEnableChannel, "ENABLE", mEnableChannel != UNDEFINED_CHANNEL );

    return true;
}

void SpiAnalyzerSettings::LoadSettings( const char* settings )
{
    SimpleArchive text_archive;
    text_archive.SetString( settings );

    const char* name_string; // the first thing in the archive is the name of the protocol analyzer that the data belongs to.
    text_archive >> &name_string;
    if( strcmp( name_string, "SaleaeSpiAnalyzer" ) != 0 )
        AnalyzerHelpers::Assert( "SaleaeSpiAnalyzer: Provided with a settings string that doesn't belong to us;" );

    text_archive >> mMosiChannel;
    text_archive >> mMisoChannel;
    text_archive >> mClockChannel;
    text_archive >> mEnableChannel;
    text_archive >> *( U32* )&mShiftOrder;
    text_archive >> mBitsPerTransfer;
    text_archive >> *( U32* )&mClockInactiveState;
    text_archive >> *( U32* )&mDataValidEdge;
    text_archive >> *( U32* )&mEnableActiveState;

    // bool success = text_archive >> mUsePackets;  //new paramater added -- do this for backwards compatibility
    // if( success == false )
    //	mUsePackets = false; //if the archive fails, set the default value

    ClearChannels();
    AddChannel( mMosiChannel, "MOSI", mMosiChannel != UNDEFINED_CHANNEL );
    AddChannel( mMisoChannel, "MISO", mMisoChannel != UNDEFINED_CHANNEL );
    AddChannel( mClockChannel, "CLOCK", mClockChannel != UNDEFINED_CHANNEL );
    AddChannel( mEnableChannel, "ENABLE", mEnableChannel != UNDEFINED_CHANNEL );

    UpdateInterfacesFromSettings();
}

const char* SpiAnalyzerSettings::SaveSettings()
{
    SimpleArchive text_archive;

    text_archive << "SaleaeSpiAnalyzer";
    text_archive << mMosiChannel;
    text_archive << mMisoChannel;
    text_archive << mClockChannel;
    text_archive << mEnableChannel;
    text_archive << mShiftOrder;
    text_archive << mBitsPerTransfer;
    text_archive << mClockInactiveState;
    text_archive << mDataValidEdge;
    text_archive << mEnableActiveState;

    return SetReturnString( text_archive.GetString() );
}

void SpiAnalyzerSettings::UpdateInterfacesFromSettings()
{
    mMosiChannelInterface->SetChannel( mMosiChannel );
    mMisoChannelInterface->SetChannel( mMisoChannel );
    mClockChannelInterface->SetChannel( mClockChannel );
    mEnableChannelInterface->SetChannel( mEnableChannel );
    mShiftOrderInterface->SetNumber( mShiftOrder );
    mBitsPerTransferInterface->SetNumber( mBitsPerTransfer );
    mClockInactiveStateInterface->SetNumber( mClockInactiveState );
    mDataValidEdgeInterface->SetNumber( mDataValidEdge );
    mEnableActiveStateInterface->SetNumber( mEnableActiveState );
}
