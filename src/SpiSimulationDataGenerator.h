#ifndef SPI_SIMULATION_DATA_GENERATOR
#define SPI_SIMULATION_DATA_GENERATOR

#include <AnalyzerHelpers.h>

class SpiAnalyzerSettings;

class SpiSimulationDataGenerator
{
  public:
    SpiSimulationDataGenerator();
    ~SpiSimulationDataGenerator();

    void Initialize( U32 simulation_sample_rate, SpiAnalyzerSettings* settings );
    U32 GenerateSimulationData( U64 newest_sample_requested, U32 sample_rate, SimulationChannelDescriptor** simulation_channels );

  protected:
    SpiAnalyzerSettings* mSettings;
    U32 mSimulationSampleRateHz;
    U64 mValue;

  protected: // SPI specific
    ClockGenerator mClockGenerator;

    void CreateSpiTransaction();
    void OutputWord_CPHA0( U64 mosi_data, U64 miso_data );
    void OutputWord_CPHA1( U64 mosi_data, U64 miso_data );


    SimulationChannelDescriptorGroup mSpiSimulationChannels;
    SimulationChannelDescriptor* mMiso;
    SimulationChannelDescriptor* mMosi;
    SimulationChannelDescriptor* mClock;
    SimulationChannelDescriptor* mEnable;
};
#endif // SPI_SIMULATION_DATA_GENERATOR
