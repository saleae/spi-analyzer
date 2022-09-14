#ifndef SPI_ANALYZER_SETTINGS
#define SPI_ANALYZER_SETTINGS

#include <AnalyzerSettings.h>
#include <AnalyzerTypes.h>

class SpiAnalyzerSettings : public AnalyzerSettings
{
  public:
    SpiAnalyzerSettings();
    virtual ~SpiAnalyzerSettings();

    virtual bool SetSettingsFromInterfaces();
    virtual void LoadSettings( const char* settings );
    virtual const char* SaveSettings();

    void UpdateInterfacesFromSettings();

    Channel mMosiChannel;
    Channel mMisoChannel;
    Channel mClockChannel;
    Channel mEnableChannel;
    AnalyzerEnums::ShiftOrder mShiftOrder;
    U32 mBitsPerTransfer;
    BitState mClockInactiveState;
    AnalyzerEnums::Edge mDataValidEdge;
    BitState mEnableActiveState;


  protected:
    std::auto_ptr<AnalyzerSettingInterfaceChannel> mMosiChannelInterface;
    std::auto_ptr<AnalyzerSettingInterfaceChannel> mMisoChannelInterface;
    std::auto_ptr<AnalyzerSettingInterfaceChannel> mClockChannelInterface;
    std::auto_ptr<AnalyzerSettingInterfaceChannel> mEnableChannelInterface;
    std::auto_ptr<AnalyzerSettingInterfaceNumberList> mShiftOrderInterface;
    std::auto_ptr<AnalyzerSettingInterfaceNumberList> mBitsPerTransferInterface;
    std::auto_ptr<AnalyzerSettingInterfaceNumberList> mClockInactiveStateInterface;
    std::auto_ptr<AnalyzerSettingInterfaceNumberList> mDataValidEdgeInterface;
    std::auto_ptr<AnalyzerSettingInterfaceNumberList> mEnableActiveStateInterface;
};

#endif // SPI_ANALYZER_SETTINGS
