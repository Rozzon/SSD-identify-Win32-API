#include <windows.h>
#include <winioctl.h>
#include <vector>
#include <ntddscsi.h>
#include <string>
#include <nvme.h>
#include <iomanip>  // Required for std::setfill, std::setw, etc
#include <sstream>

#define SENSE_LEN 32
#define DATA_LEN  512
#define ATA_IDENTIFY 0xEC
#define NVME_IDENTIFY_CNS_CONTROLLER 0x01
#define NVME_IDENTIFY_CNS_NAMESPACE  0x00
#define ATA_IDENTIFY_DATA_LENGTH 512
#define NVME_IDENTIFY_DATA_LENGTH 4096
#define IDENTIFY_SN_SIZE                      (20)
#define IDENTIFY_FR_SIZE                      (8)
#define IDENTIFY_MN_SIZE                      (40)
#define IDENTIFY_APID_SIZE                    (8)
#define IDENTIFY_CMSN_SIZE                    (60)

typedef struct _IDENTIFY_DEVICE_DATA {
    struct {
        USHORT Reserved1 : 1;
        USHORT Retired3 : 1;
        USHORT ResponseIncomplete : 1;
        USHORT Retired2 : 3;
        USHORT FixedDevice : 1;
        USHORT RemovableMedia : 1;
        USHORT Retired1 : 7;
        USHORT DeviceType : 1;
    } GeneralConfiguration;
    USHORT NumCylinders;
    USHORT SpecificConfiguration;
    USHORT NumHeads;
    USHORT Retired1[2];
    USHORT NumSectorsPerTrack;
    USHORT VendorUnique1[3];
    UCHAR  SerialNumber[20];
    USHORT Retired2[2];
    USHORT Obsolete1;
    UCHAR  FirmwareRevision[8];
    UCHAR  ModelNumber[40];
    UCHAR  MaximumBlockTransfer;
    UCHAR  VendorUnique2;
    struct {
        USHORT FeatureSupported : 1;
        USHORT Reserved : 15;
    } TrustedComputing;
    struct {
        UCHAR  CurrentLongPhysicalSectorAlignment : 2;
        UCHAR  ReservedByte49 : 6;
        UCHAR  DmaSupported : 1;
        UCHAR  LbaSupported : 1;
        UCHAR  IordyDisable : 1;
        UCHAR  IordySupported : 1;
        UCHAR  Reserved1 : 1;
        UCHAR  StandybyTimerSupport : 1;
        UCHAR  Reserved2 : 2;
        USHORT ReservedWord50;
    } Capabilities;
    USHORT ObsoleteWords51[2];
    USHORT TranslationFieldsValid : 3;
    USHORT Reserved3 : 5;
    USHORT FreeFallControlSensitivity : 8;
    USHORT NumberOfCurrentCylinders;
    USHORT NumberOfCurrentHeads;
    USHORT CurrentSectorsPerTrack;
    ULONG  CurrentSectorCapacity;
    UCHAR  CurrentMultiSectorSetting;
    UCHAR  MultiSectorSettingValid : 1;
    UCHAR  ReservedByte59 : 3;
    UCHAR  SanitizeFeatureSupported : 1;
    UCHAR  CryptoScrambleExtCommandSupported : 1;
    UCHAR  OverwriteExtCommandSupported : 1;
    UCHAR  BlockEraseExtCommandSupported : 1;
    ULONG  UserAddressableSectors;
    USHORT ObsoleteWord62;
    USHORT MultiWordDMASupport : 8;
    USHORT MultiWordDMAActive : 8;
    USHORT AdvancedPIOModes : 8;
    USHORT ReservedByte64 : 8;
    USHORT MinimumMWXferCycleTime;
    USHORT RecommendedMWXferCycleTime;
    USHORT MinimumPIOCycleTime;
    USHORT MinimumPIOCycleTimeIORDY;
    struct {
        USHORT ZonedCapabilities : 2;
        USHORT NonVolatileWriteCache : 1;
        USHORT ExtendedUserAddressableSectorsSupported : 1;
        USHORT DeviceEncryptsAllUserData : 1;
        USHORT ReadZeroAfterTrimSupported : 1;
        USHORT Optional28BitCommandsSupported : 1;
        USHORT IEEE1667 : 1;
        USHORT DownloadMicrocodeDmaSupported : 1;
        USHORT SetMaxSetPasswordUnlockDmaSupported : 1;
        USHORT WriteBufferDmaSupported : 1;
        USHORT ReadBufferDmaSupported : 1;
        USHORT DeviceConfigIdentifySetDmaSupported : 1;
        USHORT LPSAERCSupported : 1;
        USHORT DeterministicReadAfterTrimSupported : 1;
        USHORT CFastSpecSupported : 1;
    } AdditionalSupported;
    USHORT ReservedWords70[5];
    USHORT QueueDepth : 5;
    USHORT ReservedWord75 : 11;
    struct {
        USHORT Reserved0 : 1;
        USHORT SataGen1 : 1;
        USHORT SataGen2 : 1;
        USHORT SataGen3 : 1;
        USHORT Reserved1 : 4;
        USHORT NCQ : 1;
        USHORT HIPM : 1;
        USHORT PhyEvents : 1;
        USHORT NcqUnload : 1;
        USHORT NcqPriority : 1;
        USHORT HostAutoPS : 1;
        USHORT DeviceAutoPS : 1;
        USHORT ReadLogDMA : 1;
        USHORT Reserved2 : 1;
        USHORT CurrentSpeed : 3;
        USHORT NcqStreaming : 1;
        USHORT NcqQueueMgmt : 1;
        USHORT NcqReceiveSend : 1;
        USHORT DEVSLPtoReducedPwrState : 1;
        USHORT Reserved3 : 8;
    } SerialAtaCapabilities;
    struct {
        USHORT Reserved0 : 1;
        USHORT NonZeroOffsets : 1;
        USHORT DmaSetupAutoActivate : 1;
        USHORT DIPM : 1;
        USHORT InOrderData : 1;
        USHORT HardwareFeatureControl : 1;
        USHORT SoftwareSettingsPreservation : 1;
        USHORT NCQAutosense : 1;
        USHORT DEVSLP : 1;
        USHORT HybridInformation : 1;
        USHORT Reserved1 : 6;
    } SerialAtaFeaturesSupported;
    struct {
        USHORT Reserved0 : 1;
        USHORT NonZeroOffsets : 1;
        USHORT DmaSetupAutoActivate : 1;
        USHORT DIPM : 1;
        USHORT InOrderData : 1;
        USHORT HardwareFeatureControl : 1;
        USHORT SoftwareSettingsPreservation : 1;
        USHORT DeviceAutoPS : 1;
        USHORT DEVSLP : 1;
        USHORT HybridInformation : 1;
        USHORT Reserved1 : 6;
    } SerialAtaFeaturesEnabled;
    USHORT MajorRevision;
    USHORT MinorRevision;
    struct {
        USHORT SmartCommands : 1;
        USHORT SecurityMode : 1;
        USHORT RemovableMediaFeature : 1;
        USHORT PowerManagement : 1;
        USHORT Reserved1 : 1;
        USHORT WriteCache : 1;
        USHORT LookAhead : 1;
        USHORT ReleaseInterrupt : 1;
        USHORT ServiceInterrupt : 1;
        USHORT DeviceReset : 1;
        USHORT HostProtectedArea : 1;
        USHORT Obsolete1 : 1;
        USHORT WriteBuffer : 1;
        USHORT ReadBuffer : 1;
        USHORT Nop : 1;
        USHORT Obsolete2 : 1;
        USHORT DownloadMicrocode : 1;
        USHORT DmaQueued : 1;
        USHORT Cfa : 1;
        USHORT AdvancedPm : 1;
        USHORT Msn : 1;
        USHORT PowerUpInStandby : 1;
        USHORT ManualPowerUp : 1;
        USHORT Reserved2 : 1;
        USHORT SetMax : 1;
        USHORT Acoustics : 1;
        USHORT BigLba : 1;
        USHORT DeviceConfigOverlay : 1;
        USHORT FlushCache : 1;
        USHORT FlushCacheExt : 1;
        USHORT WordValid83 : 2;
        USHORT SmartErrorLog : 1;
        USHORT SmartSelfTest : 1;
        USHORT MediaSerialNumber : 1;
        USHORT MediaCardPassThrough : 1;
        USHORT StreamingFeature : 1;
        USHORT GpLogging : 1;
        USHORT WriteFua : 1;
        USHORT WriteQueuedFua : 1;
        USHORT WWN64Bit : 1;
        USHORT URGReadStream : 1;
        USHORT URGWriteStream : 1;
        USHORT ReservedForTechReport : 2;
        USHORT IdleWithUnloadFeature : 1;
        USHORT WordValid : 2;
    } CommandSetSupport;
    struct {
        USHORT SmartCommands : 1;
        USHORT SecurityMode : 1;
        USHORT RemovableMediaFeature : 1;
        USHORT PowerManagement : 1;
        USHORT Reserved1 : 1;
        USHORT WriteCache : 1;
        USHORT LookAhead : 1;
        USHORT ReleaseInterrupt : 1;
        USHORT ServiceInterrupt : 1;
        USHORT DeviceReset : 1;
        USHORT HostProtectedArea : 1;
        USHORT Obsolete1 : 1;
        USHORT WriteBuffer : 1;
        USHORT ReadBuffer : 1;
        USHORT Nop : 1;
        USHORT Obsolete2 : 1;
        USHORT DownloadMicrocode : 1;
        USHORT DmaQueued : 1;
        USHORT Cfa : 1;
        USHORT AdvancedPm : 1;
        USHORT Msn : 1;
        USHORT PowerUpInStandby : 1;
        USHORT ManualPowerUp : 1;
        USHORT Reserved2 : 1;
        USHORT SetMax : 1;
        USHORT Acoustics : 1;
        USHORT BigLba : 1;
        USHORT DeviceConfigOverlay : 1;
        USHORT FlushCache : 1;
        USHORT FlushCacheExt : 1;
        USHORT Resrved3 : 1;
        USHORT Words119_120Valid : 1;
        USHORT SmartErrorLog : 1;
        USHORT SmartSelfTest : 1;
        USHORT MediaSerialNumber : 1;
        USHORT MediaCardPassThrough : 1;
        USHORT StreamingFeature : 1;
        USHORT GpLogging : 1;
        USHORT WriteFua : 1;
        USHORT WriteQueuedFua : 1;
        USHORT WWN64Bit : 1;
        USHORT URGReadStream : 1;
        USHORT URGWriteStream : 1;
        USHORT ReservedForTechReport : 2;
        USHORT IdleWithUnloadFeature : 1;
        USHORT Reserved4 : 2;
    } CommandSetActive;
    USHORT UltraDMASupport : 8;
    USHORT UltraDMAActive : 8;
    struct {
        USHORT TimeRequired : 15;
        USHORT ExtendedTimeReported : 1;
    } NormalSecurityEraseUnit;
    struct {
        USHORT TimeRequired : 15;
        USHORT ExtendedTimeReported : 1;
    } EnhancedSecurityEraseUnit;
    USHORT CurrentAPMLevel : 8;
    USHORT ReservedWord91 : 8;
    USHORT MasterPasswordID;
    USHORT HardwareResetResult;
    USHORT CurrentAcousticValue : 8;
    USHORT RecommendedAcousticValue : 8;
    USHORT StreamMinRequestSize;
    USHORT StreamingTransferTimeDMA;
    USHORT StreamingAccessLatencyDMAPIO;
    ULONG  StreamingPerfGranularity;
    ULONG  Max48BitLBA[2];
    USHORT StreamingTransferTime;
    USHORT DsmCap;
    struct {
        USHORT LogicalSectorsPerPhysicalSector : 4;
        USHORT Reserved0 : 8;
        USHORT LogicalSectorLongerThan256Words : 1;
        USHORT MultipleLogicalSectorsPerPhysicalSector : 1;
        USHORT Reserved1 : 2;
    } PhysicalLogicalSectorSize;
    USHORT InterSeekDelay;
    USHORT WorldWideName[4];
    USHORT ReservedForWorldWideName128[4];
    USHORT ReservedForTlcTechnicalReport;
    USHORT WordsPerLogicalSector[2];
    struct {
        USHORT ReservedForDrqTechnicalReport : 1;
        USHORT WriteReadVerify : 1;
        USHORT WriteUncorrectableExt : 1;
        USHORT ReadWriteLogDmaExt : 1;
        USHORT DownloadMicrocodeMode3 : 1;
        USHORT FreefallControl : 1;
        USHORT SenseDataReporting : 1;
        USHORT ExtendedPowerConditions : 1;
        USHORT Reserved0 : 6;
        USHORT WordValid : 2;
    } CommandSetSupportExt;
    struct {
        USHORT ReservedForDrqTechnicalReport : 1;
        USHORT WriteReadVerify : 1;
        USHORT WriteUncorrectableExt : 1;
        USHORT ReadWriteLogDmaExt : 1;
        USHORT DownloadMicrocodeMode3 : 1;
        USHORT FreefallControl : 1;
        USHORT SenseDataReporting : 1;
        USHORT ExtendedPowerConditions : 1;
        USHORT Reserved0 : 6;
        USHORT Reserved1 : 2;
    } CommandSetActiveExt;
    USHORT ReservedForExpandedSupportandActive[6];
    USHORT MsnSupport : 2;
    USHORT ReservedWord127 : 14;
    struct {
        USHORT SecuritySupported : 1;
        USHORT SecurityEnabled : 1;
        USHORT SecurityLocked : 1;
        USHORT SecurityFrozen : 1;
        USHORT SecurityCountExpired : 1;
        USHORT EnhancedSecurityEraseSupported : 1;
        USHORT Reserved0 : 2;
        USHORT SecurityLevel : 1;
        USHORT Reserved1 : 7;
    } SecurityStatus;
    USHORT ReservedWord129[31];
    struct {
        USHORT MaximumCurrentInMA : 12;
        USHORT CfaPowerMode1Disabled : 1;
        USHORT CfaPowerMode1Required : 1;
        USHORT Reserved0 : 1;
        USHORT Word160Supported : 1;
    } CfaPowerMode1;
    USHORT ReservedForCfaWord161[7];
    USHORT NominalFormFactor : 4;
    USHORT ReservedWord168 : 12;
    struct {
        USHORT SupportsTrim : 1;
        USHORT Reserved0 : 15;
    } DataSetManagementFeature;
    USHORT AdditionalProductID[4];
    USHORT ReservedForCfaWord174[2];
    USHORT CurrentMediaSerialNumber[30];
    struct {
        USHORT Supported : 1;
        USHORT Reserved0 : 1;
        USHORT WriteSameSuported : 1;
        USHORT ErrorRecoveryControlSupported : 1;
        USHORT FeatureControlSuported : 1;
        USHORT DataTablesSuported : 1;
        USHORT Reserved1 : 6;
        USHORT VendorSpecific : 4;
    } SCTCommandTransport;
    USHORT ReservedWord207[2];
    struct {
        USHORT AlignmentOfLogicalWithinPhysical : 14;
        USHORT Word209Supported : 1;
        USHORT Reserved0 : 1;
    } BlockAlignment;
    USHORT WriteReadVerifySectorCountMode3Only[2];
    USHORT WriteReadVerifySectorCountMode2Only[2];
    struct {
        USHORT NVCachePowerModeEnabled : 1;
        USHORT Reserved0 : 3;
        USHORT NVCacheFeatureSetEnabled : 1;
        USHORT Reserved1 : 3;
        USHORT NVCachePowerModeVersion : 4;
        USHORT NVCacheFeatureSetVersion : 4;
    } NVCacheCapabilities;
    USHORT NVCacheSizeLSW;
    USHORT NVCacheSizeMSW;
    USHORT NominalMediaRotationRate;
    USHORT ReservedWord218;
    struct {
        UCHAR NVCacheEstimatedTimeToSpinUpInSeconds;
        UCHAR Reserved;
    } NVCacheOptions;
    USHORT WriteReadVerifySectorCountMode : 8;
    USHORT ReservedWord220 : 8;
    USHORT ReservedWord221;
    struct {
        USHORT MajorVersion : 12;
        USHORT TransportType : 4;
    } TransportMajorVersion;
    USHORT TransportMinorVersion;
    USHORT ReservedWord224[6];
    ULONG  ExtendedNumberOfUserAddressableSectors[2];
    USHORT MinBlocksPerDownloadMicrocodeMode03;
    USHORT MaxBlocksPerDownloadMicrocodeMode03;
    USHORT ReservedWord236[19];
    USHORT Signature : 8;
    USHORT CheckSum : 8;
} IDENTIFY_DEVICE_DATA, * PIDENTIFY_DEVICE_DATA;

typedef struct _SCSI_PASS_THROUGH_WITH_BUFFERS {
    SCSI_PASS_THROUGH spt;          // SCSI_PASS_THROUGH structure
    ULONG             Filler;       // Padding to align the buffer properly
    UCHAR             ucSenseBuf[SENSE_LEN];  // Buffer to hold the sense data
    UCHAR             ucDataBuf[DATA_LEN];    // Buffer to hold the data returned by the device
} SCSI_PASS_THROUGH_WITH_BUFFERS, * PSCSI_PASS_THROUGH_WITH_BUFFERS;

// Global stringstream to store output
std::stringstream ss;

#ifdef _DEBUG
void DebugPrint(const char* format, ...) {
    char buffer[1024]; // Adjust buffer size as needed

    va_list args;
    va_start(args, format);
    vsprintf_s(buffer, sizeof(buffer), format, args);
    va_end(args);

    OutputDebugStringA(buffer);
}
#else
void DebugPrint(const char* format, ...) {
    // Empty or define a no-op for release builds
}
#endif

void FormatNvmeIdentifyField(const NVME_IDENTIFY_CONTROLLER_DATA* data) {
    ss << ">>>>>>>>>>>>>>>>>>>>>NVMe Identify Start\r\n";

    ss << "byte 0:1     M - PCI Vendor ID (VID): 0x" << std::hex << std::setw(4) << std::setfill('0') << data->VID << std::dec << "\r\n";
    ss << "byte 2:3     M - PCI Subsystem Vendor ID (SSVID): 0x" << std::hex << std::setw(4) << std::setfill('0') << data->SSVID << std::dec << "\r\n";

    ss << "byte 24:63   M - Model Number (MN): " << std::string(reinterpret_cast<const char*>(data->MN), 40) << "\r\n";
    ss << "byte 4: 23   M - Serial Number (SN): " << std::string(reinterpret_cast<const char*>(data->SN), 20) << "\r\n";
    ss << "byte 64:71   M - Firmware Revision(FR) : " << std::string(reinterpret_cast<const char*>(data->FR), 8) << "\r\n";

    ss << "byte 72      M - Recommended Arbitration Burst (RAB): 0x" << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(data->RAB) << std::dec << "\r\n";
    ss << "byte 73:75   M - IEEE OUI Identifier (IEEE). Controller Vendor code.: 0x";
    for (int i = 0; i < sizeof(data->IEEE); ++i) {
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(data->IEEE[i]);
    }
    ss << std::dec << "\r\n";

    ss << "byte 76      O - Controller Multi-Path I/O and Namespace Sharing Capabilities (CMIC):\r\n";
    ss << "  bit 0      Multi PCIe Ports: " << static_cast<int>(data->CMIC.MultiPCIePorts) << "\r\n";
    ss << "  bit 1      Multi Controllers: " << static_cast<int>(data->CMIC.MultiControllers) << "\r\n";
    ss << "  bit 2      SRIOV: " << static_cast<int>(data->CMIC.SRIOV) << "\r\n";
    ss << "  bit 3      ANAR: " << static_cast<int>(data->CMIC.ANAR) << "\r\n";
    ss << "  bit 4..7   Reserved: " << static_cast<int>(data->CMIC.Reserved) << "\r\n";

    ss << "byte 77      M - Maximum Data Transfer Size (MDTS): " << static_cast<int>(data->MDTS) << "\r\n";
    ss << "byte 78:79   M - Controller ID (CNTLID): 0x" << std::hex << std::setw(4) << std::setfill('0') << data->CNTLID << std::dec << "\r\n";
    ss << "byte 80:83   M - Version (VER): 0x" << std::hex << std::setw(8) << std::setfill('0') << data->VER << std::dec << "\r\n";

    ss << "byte 84:87   M - RTD3 Resume Latency (RTD3R): 0x" << std::hex << std::setw(8) << std::setfill('0') << data->RTD3R << std::dec << "\r\n";
    ss << "byte 88:91   M - RTD3 Entry Latency (RTD3E): 0x" << std::hex << std::setw(8) << std::setfill('0') << data->RTD3E << std::dec << "\r\n";

    // OAES and CTRATT are bit fields, extract and format their values
    ss << "byte 92:95   M - Optional Asynchronous Events Supported (OAES):\r\n";
    ss << "  bit 0..7   Reserved0: " << static_cast<int>(data->OAES.Reserved0) << "\r\n";
    ss << "  bit 8      Namespace Attribute Changed: " << static_cast<int>(data->OAES.NamespaceAttributeChanged) << "\r\n";
    ss << "  bit 9      Firmware Activation: " << static_cast<int>(data->OAES.FirmwareActivation) << "\r\n";
    ss << "  bit 10     Reserved1: " << static_cast<int>(data->OAES.FirmwareActivation) << "\r\n";
    ss << "  bit 11     Asymmetric Access Changed: " << static_cast<int>(data->OAES.AsymmetricAccessChanged) << "\r\n";
    ss << "  bit 12     Predictable Latency Aggregate Log Changed: " << static_cast<int>(data->OAES.PredictableLatencyAggregateLogChanged) << "\r\n";
    ss << "  bit 13     LBA Status Changed: " << static_cast<int>(data->OAES.LbaStatusChanged) << "\r\n";
    ss << "  bit 14     Endurance Group Aggregate Log Changed: " << static_cast<int>(data->OAES.EnduranceGroupAggregateLogChanged) << "\r\n";
    ss << "  bit 15..26 Reserved2: " << static_cast<int>(data->OAES.Reserved2) << "\r\n";
    ss << "  bit 27     Zone Information: " << static_cast<int>(data->OAES.ZoneInformation) << "\r\n";

    ss << "byte 96:99   M - Controller Attributes (CTRATT):\r\n";
    ss << "  bit 0      Host Identifier 128-bit: " << static_cast<int>(data->CTRATT.HostIdentifier128Bit) << "\r\n";
    ss << "  bit 1      NOPSP Mode: " << static_cast<int>(data->CTRATT.NOPSPMode) << "\r\n";
    ss << "  bit 2      NVMe Sets: " << static_cast<int>(data->CTRATT.NVMSets) << "\r\n";
    ss << "  bit 3      Read Recovery Levels: " << static_cast<int>(data->CTRATT.ReadRecoveryLevels) << "\r\n";
    ss << "  bit 4      Endurance Groups: " << static_cast<int>(data->CTRATT.EnduranceGroups) << "\r\n";
    ss << "  bit 5      Predictable Latency Mode: " << static_cast<int>(data->CTRATT.PredictableLatencyMode) << "\r\n";
    ss << "  bit 6      Traffic Based Keep Alive Support(TBKAS): " << static_cast<int>(data->CTRATT.TBKAS) << "\r\n";
    ss << "  bit 7      Namespace Granularity: " << static_cast<int>(data->CTRATT.NamespaceGranularity) << "\r\n";
    ss << "  bit 8      SQA ssociations: " << static_cast<int>(data->CTRATT.SQAssociations) << "\r\n";
    ss << "  bit 9      UUID List: " << static_cast<int>(data->CTRATT.UUIDList) << "\r\n";
    ss << "  bit 10..31 Reserved0: " << static_cast<int>(data->CTRATT.Reserved0) << "\r\n";

    ss << "byte 100:101 O - Read Recovery Levels Supported (RRLS):\r\n";
    ss << "  bit 0      ReadRecoveryLevel0: " << static_cast<int>(data->RRLS.ReadRecoveryLevel0) << "\r\n";
    ss << "  bit 1      ReadRecoveryLevel1: " << static_cast<int>(data->RRLS.ReadRecoveryLevel1) << "\r\n";
    ss << "  bit 2      ReadRecoveryLevel2: " << static_cast<int>(data->RRLS.ReadRecoveryLevel2) << "\r\n";
    ss << "  bit 3      ReadRecoveryLevel3: " << static_cast<int>(data->RRLS.ReadRecoveryLevel3) << "\r\n";
    ss << "  bit 4      ReadRecoveryLevel4: " << static_cast<int>(data->RRLS.ReadRecoveryLevel4) << "\r\n";
    ss << "  bit 5      ReadRecoveryLevel5: " << static_cast<int>(data->RRLS.ReadRecoveryLevel5) << "\r\n";
    ss << "  bit 6      ReadRecoveryLevel6: " << static_cast<int>(data->RRLS.ReadRecoveryLevel6) << "\r\n";
    ss << "  bit 7      ReadRecoveryLevel7: " << static_cast<int>(data->RRLS.ReadRecoveryLevel7) << "\r\n";
    ss << "  bit 8      ReadRecoveryLevel8: " << static_cast<int>(data->RRLS.ReadRecoveryLevel8) << "\r\n";
    ss << "  bit 9      ReadRecoveryLevel9: " << static_cast<int>(data->RRLS.ReadRecoveryLevel9) << "\r\n";
    ss << "  bit 10     ReadRecoveryLeve110: " << static_cast<int>(data->RRLS.ReadRecoveryLevel10) << "\r\n";
    ss << "  bit 11     ReadRecoveryLevel11: " << static_cast<int>(data->RRLS.ReadRecoveryLevel11) << "\r\n";
    ss << "  bit 12     ReadRecoveryLevel12: " << static_cast<int>(data->RRLS.ReadRecoveryLevel12) << "\r\n";
    ss << "  bit 13     ReadRecoveryLevel13: " << static_cast<int>(data->RRLS.ReadRecoveryLevel13) << "\r\n";
    ss << "  bit 14     ReadRecoveryLevel14: " << static_cast<int>(data->RRLS.ReadRecoveryLevel14) << "\r\n";
    ss << "  bit 15     ReadRecoveryLevel15: " << static_cast<int>(data->RRLS.ReadRecoveryLevel15) << "\r\n";

    ss << "byte 102:110 Reserved:\r\n";

    ss << "byte 111     M - Controller Type: 0x" << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(data->CNTRLTYPE) << std::dec << "\r\n";
    // FGUID field formatting
    ss << "byte 112..127  FRU Globally Unique Identifier (FGUID): ";
    for (int i = 0; i < 16; ++i) {
        ss << std::hex << std::setw(1) << static_cast<int>(data->FGUID[i]);
    }
    ss << std::dec;
    ss << "\r\n";

    ss << "byte 128:129  O - Command Retry Delay Time 1(CRDT1) : " << static_cast<int>(data->CRDT1) << "\r\n";
    ss << "byte 130:131  O - Command Retry Delay Time 2(CRDT2) : " << static_cast<int>(data->CRDT2) << "\r\n";
    ss << "byte 132:133  O - Command Retry Delay Time 3(CRDT3) : " << static_cast<int>(data->CRDT3) << "\r\n";

    ss << "byte 134:239  Reserved:\r\n";

    ss << "byte 240:255  Refer to the NVMe Management Interface Specification for definition: ";
    for (int i = 0; i < 16; ++i) {
        ss << std::hex << std::setw(2) << std::setfill('0')
            << static_cast<int>(data->ReservedForManagement[i]) << ' ';
    }
    ss << std::dec; // Switch back to decimal format if needed
    ss << "\r\n";

    ss << "byte 256:257 M - Optional Admin Command Set Attributes (OACS):\r\n";
    ss << "  bit 0      Security Commands: " << static_cast<int>(data->OACS.SecurityCommands) << "\r\n";
    ss << "  bit 1      Format NVM: " << static_cast<int>(data->OACS.FormatNVM) << "\r\n";
    ss << "  bit 2      Firmware Commands: " << static_cast<int>(data->OACS.FirmwareCommands) << "\r\n";
    ss << "  bit 3      Namespace Commands: " << static_cast<int>(data->OACS.NamespaceCommands) << "\r\n";
    ss << "  bit 4      Device Self Test: " << static_cast<int>(data->OACS.DeviceSelfTest) << "\r\n";
    ss << "  bit 5      Directives: " << static_cast<int>(data->OACS.Directives) << "\r\n";
    ss << "  bit 6      NVMe Management Interface Commands: " << static_cast<int>(data->OACS.NVMeMICommands) << "\r\n";
    ss << "  bit 7      Virtualization Management: " << static_cast<int>(data->OACS.VirtualizationMgmt) << "\r\n";
    ss << "  bit 8      Door Bell Buffer Configuration: " << static_cast<int>(data->OACS.DoorBellBufferConfig) << "\r\n";
    ss << "  bit 9      Get LBA Status: " << static_cast<int>(data->OACS.GetLBAStatus) << "\r\n";
    ss << "  bit 10..15 Reserved: " << std::hex << std::setw(2) << std::setfill('0')
        << static_cast<int>(data->OACS.Reserved) << std::dec << "\r\n";

    ss << "  byte 258   M - Abort Command Limit (ACL): " << static_cast<int>(data->ACL) << "\r\n";
    ss << "  byte 259   M - Asynchronous Event Request Limit (AERL): " << static_cast<int>(data->AERL) << "\r\n";

    ss << "byte 260     M - Firmware Updates (FRMW):\r\n";
    ss << "  bit 0      Slot1 ReadOnly: " << static_cast<int>(data->FRMW.Slot1ReadOnly) << "\r\n";
    ss << "  bit 1..3   Slot Count: " << static_cast<int>(data->FRMW.SlotCount) << "\r\n";
    ss << "  bit 4      ActivationWithout Reset: " << static_cast<int>(data->FRMW.ActivationWithoutReset) << "\r\n";
    ss << "  bit 5..7   Reserved: " << static_cast<int>(data->FRMW.Reserved) << "\r\n";

    ss << "byte 261     M - Log Page Attributes (LPA):\r\n";
    ss << "  bit 0      Smart Page Per Namespace: " << static_cast<int>(data->LPA.SmartPagePerNamespace) << "\r\n";
    ss << "  bit 1      Command Effects Log: " << static_cast<int>(data->LPA.CommandEffectsLog) << "\r\n";
    ss << "  bit 2      Log Page Extended Data: " << static_cast<int>(data->LPA.LogPageExtendedData) << "\r\n";
    ss << "  bit 3      Telemetry Support: " << static_cast<int>(data->LPA.TelemetrySupport) << "\r\n";
    ss << "  bit 4      Persistent Event Log: " << static_cast<int>(data->LPA.PersistentEventLog) << "\r\n";
    ss << "  bit 5      Reserved0: " << static_cast<int>(data->LPA.Reserved0) << "\r\n";
    ss << "  bit 6      Telemetry Data Area4: " << static_cast<int>(data->LPA.TelemetryDataArea4) << "\r\n";
    ss << "  bit 7      Reserved1: " << static_cast<int>(data->LPA.Reserved1) << "\r\n";

    ss << "byte 262     M - Error Log Page Entries (ELPE): " << static_cast<int>(data->ELPE) << "\r\n";
    ss << "byte 263     M - Number of Power States Support (NPSS): " << static_cast<int>(data->NPSS) << "\r\n";

    ss << "byte 264     M - Admin Vendor Specific Command Configuration (AVSCC):\r\n";
    ss << "  bit 0      Command Format in Spec: " << static_cast<int>(data->AVSCC.CommandFormatInSpec) << "\r\n";
    ss << "  bit 1..7   Reserved: " << static_cast<int>(data->AVSCC.Reserved) << "\r\n";

    ss << "byte 265     O - Autonomous Power State Transition Attributes (APSTA):\r\n";
    ss << "  bit 0      Supported: " << static_cast<int>(data->APSTA.Supported) << "\r\n";
    ss << "  bit 1..7   Reserved: " << static_cast<int>(data->APSTA.Reserved) << "\r\n";

    ss << "byte 266:267  M - Warning Composite Temperature Threshold (WCTEMP): " << data->WCTEMP << "\r\n";
    ss << "byte 268:269  M - Critical Composite Temperature Threshold (CCTEMP):" << data->CCTEMP << "\r\n";
    ss << "byte 270:271  O - Maximum Time for Firmware Activation (MTFA):" << data->MTFA << "\r\n";
    ss << "byte 272:275  O - Host Memory Buffer Preferred Size (HMPRE):" << data->HMPRE << "\r\n";
    ss << "byte 276:279  O - Host Memory Buffer Minimum Size (HMMIN): " << data->HMMIN << "\r\n";

    ss << "byte 280:295  O - Total NVM Capacity (TNVMCAP): ";
    for (size_t i = 0; i < 16; ++i) {
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(data->TNVMCAP[i]) << ' ';
    }
    ss << "\r\n";

    ss << "byte 296:311  O - Unallocated NVM Capacity (UNVMCAP): ";
    for (int i = 0; i < 16; ++i) {
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(data->UNVMCAP[i]) << ' ';
    }
    ss << "\r\n";

    ss << "byte 312:315  O - Replay Protected Memory Block Support (RPMBS):\r\n";
    ss << "  bit 0..2    RPMB Unit Count: " << static_cast<int>(data->RPMBS.RPMBUnitCount) << "\r\n";
    ss << "  bit 3..5    Authentication Method: " << static_cast<int>(data->RPMBS.AuthenticationMethod) << "\r\n";
    ss << "  bit 6..15   Reserved0: " << static_cast<int>(data->RPMBS.Reserved0) << "\r\n";
    ss << "  bit 16..23  Total Size: " << static_cast<int>(data->RPMBS.TotalSize) << " (in 128KB units)\r\n";
    ss << "  bit 24..31  Access Size: " << static_cast<int>(data->RPMBS.AccessSize) << " (in 512B units)\r\n";

    ss << "byte 316:317  O - Extended Device Self-test Time (EDSTT): " << data->EDSTT << "\r\n";
    ss << "byte 318      O - Device Self-test Options (DSTO): " << static_cast<int>(data->DSTO) << "\r\n";
    ss << "byte 319      M - Firmware Update Granularity (FWUG): " << static_cast<int>(data->FWUG) << "\r\n";
    ss << "byte 320:321  M - Keep Alive Support (KAS): " << data->KAS << "\r\n";

    ss << "byte 322:323  O - Host Controlled Thermal Management Attributes (HCTMA):\r\n";
    ss << "  bit 0       Supported: " << static_cast<int>(data->HCTMA.Supported) << "\r\n";
    ss << "  bit 1..15   Reserved: " << static_cast<int>(data->HCTMA.Reserved) << "\r\n";

    ss << "byte 324:325  O - Minimum Thermal Management Temperature (MNTMT): "
        << std::hex << std::setw(4) << std::setfill('0') << static_cast<int>(data->MNTMT) << std::dec << "\r\n";
    ss << "byte 326:327  O - Maximum Thermal Management Temperature (MXTMT): "
        << std::hex << std::setw(4) << std::setfill('0') << static_cast<int>(data->MXTMT) << std::dec << "\r\n";

    ss << "byte 328:331  O - Sanitize Capabilities (SANICAP):\r\n";
    ss << "  bit 0       Crypto Erase: " << static_cast<int>(data->SANICAP.CryptoErase) << "\r\n";
    ss << "  bit 1       Block Erase: " << static_cast<int>(data->SANICAP.BlockErase) << "\r\n";
    ss << "  bit 2       Overwrite: " << static_cast<int>(data->SANICAP.Overwrite) << "\r\n";
    ss << "  bit 3..28   Reserved: " << static_cast<int>(data->SANICAP.Reserved) << "\r\n";
    ss << "  bit 29      No-Deallocate Inhibited (NDI): " << static_cast<int>(data->SANICAP.NDI) << "\r\n";
    ss << "  bit 30..31  No-Deallocate Modifies Media After Sanitize (NODMMAS): " << static_cast<int>(data->SANICAP.NODMMAS) << "\r\n";

    ss << "byte 332:335  O - Host Memory Buffer Minimum Descriptor Entry Size (HMMINDS): " << data->HMMINDS << "\r\n";
    ss << "byte 336:337  O - Host Memory Maximum Descriptors Entries (HMMAXD): " << data->HMMAXD << "\r\n";
    ss << "byte 338:339  O - NVM Set Identifier Maximum (NSETIDMAX): " << data->NSETIDMAX << "\r\n";
    ss << "byte 340:341  O - Endurance Group Identifier Maximum (ENDGIDMAX): " << data->ENDGIDMAX << "\r\n";
    ss << "byte 342      O - ANA Transition Time (ANATT): " << static_cast<int>(data->ANATT) << "\r\n";

    ss << "byte 343      O - Asymmetric Namespace Access Capabilities (ANACAP):\r\n";
    ss << "  bit 0       Optimized State: " << static_cast<int>(data->ANACAP.OptimizedState) << "\r\n";
    ss << "  bit 1       Non-Optimized State: " << static_cast<int>(data->ANACAP.NonOptimizedState) << "\r\n";
    ss << "  bit 2       Inaccessible State: " << static_cast<int>(data->ANACAP.InaccessibleState) << "\r\n";
    ss << "  bit 3       Persistent Loss State: " << static_cast<int>(data->ANACAP.PersistentLossState) << "\r\n";
    ss << "  bit 4       Change State: " << static_cast<int>(data->ANACAP.ChangeState) << "\r\n";
    ss << "  bit 5       Reserved: " << static_cast<int>(data->ANACAP.Reserved) << "\r\n";
    ss << "  bit 6       Static ANAGRPID: " << static_cast<int>(data->ANACAP.StaticANAGRPID) << "\r\n";
    ss << "  bit 7       Support Non-Zero ANAGRPID: " << static_cast<int>(data->ANACAP.SupportNonZeroANAGRPID) << "\r\n";

    ss << "byte 344:347  O - ANA Group Identifier Maximum (ANAGRPMAX): " << data->ANAGRPMAX << "\r\n";
    ss << "byte 348:351  O - Number of ANA Group Identifiers (NANAGRPID): " << data->NANAGRPID << "\r\n";
    ss << "byte 352:355  O - Persistent Event Log Size (PELS): " << data->PELS << "\r\n";
    ss << "byte 356:511  Reserved: \r\n";

    ss << "byte 512      M - Submission Queue Entry Size (SQES):\r\n";
    ss << "  Required Entry Size: " << static_cast<int>(data->SQES.RequiredEntrySize) << " (2^" << static_cast<int>(data->SQES.RequiredEntrySize) << " bytes)\r\n";
    ss << "  Max Entry Size: " << static_cast<int>(data->SQES.MaxEntrySize) << " (2^" << static_cast<int>(data->SQES.MaxEntrySize) << " bytes)\r\n";

    ss << "byte 513      M - Completion Queue Entry Size (CQES):\r\n";
    ss << "  Required Entry Size: " << static_cast<int>(data->CQES.RequiredEntrySize) << " (2^" << static_cast<int>(data->CQES.RequiredEntrySize) << " bytes)\r\n";
    ss << "  Max Entry Size: " << static_cast<int>(data->CQES.MaxEntrySize) << " (2^" << static_cast<int>(data->CQES.MaxEntrySize) << " bytes)\r\n";

    ss << "byte 514:515  M - Maximum Outstanding Commands (MAXCMD): " << data->MAXCMD << "\r\n";
    ss << "byte 516:519  M - Number of Namespaces (NN): " << data->NN << "\r\n";

    ss << "byte 520:521 M - Optional NVM Command Support (ONCS):\r\n";
    ss << "  bit 0 Compare: " << static_cast<int>(data->ONCS.Compare) << "\r\n";
    ss << "  bit 1 Write Uncorrectable: " << static_cast<int>(data->ONCS.WriteUncorrectable) << "\r\n";
    ss << "  bit 2 Dataset Management: " << static_cast<int>(data->ONCS.DatasetManagement) << "\r\n";
    ss << "  bit 3 Write Zeroes: " << static_cast<int>(data->ONCS.WriteZeroes) << "\r\n";
    ss << "  bit 4 Feature Field: " << static_cast<int>(data->ONCS.FeatureField) << "\r\n";
    ss << "  bit 5 Reservations: " << static_cast<int>(data->ONCS.Reservations) << "\r\n";
    ss << "  bit 6 Timestamp: " << static_cast<int>(data->ONCS.Timestamp) << "\r\n";
    ss << "  bit 7 Verify: " << static_cast<int>(data->ONCS.Verify) << "\r\n";
    ss << "  bit 8..15 Reserved: " << static_cast<int>(data->ONCS.Reserved) << "\r\n";

    ss << "byte 522:523 M - Fused Operation Support (FUSES):\r\n";
    ss << "  bit 0 Compare and Write: " << static_cast<int>(data->FUSES.CompareAndWrite) << "\r\n";
    ss << "  bit 1..15 Reserved: " << static_cast<int>(data->FUSES.Reserved) << "\r\n";

    ss << "byte 524     M - Format NVM Attributes (FNA):\r\n";
    ss << "  bit 0      Format Apply To All: " << static_cast<int>(data->FNA.FormatApplyToAll) << "\r\n";
    ss << "  bit 1      Secure Erase Apply To All: " << static_cast<int>(data->FNA.SecureEraseApplyToAll) << "\r\n";
    ss << "  bit 2      Cryptographic Erase Supported: " << static_cast<int>(data->FNA.CryptographicEraseSupported) << "\r\n";
    ss << "  bit 3      Format Support NSID All F: " << static_cast<int>(data->FNA.FormatSupportNSIDAllF) << "\r\n";
    ss << "  bit 4..7   Reserved: " << static_cast<int>(data->FNA.Reserved) << "\r\n";

    ss << "byte 525     M - Volatile Write Cache (VWC):\r\n";
    ss << "  bit 0      Present: " << static_cast<int>(data->VWC.Present) << "\r\n";
    ss << "  bit 1..2   Flush Behavior: " << static_cast<int>(data->VWC.FlushBehavior) << "\r\n";
    ss << "  bit 3..7   Reserved: " << static_cast<int>(data->VWC.Reserved) << "\r\n";

    ss << "byte 526:527 M - Atomic Write Unit Normal (AWUN): " << data->AWUN << "\r\n";
    ss << "byte 528:529 M - Atomic Write Unit Power Fail (AWUPF): " << data->AWUPF << "\r\n";

    ss << "byte 530     M - NVM Vendor Specific Command Configuration (NVSCC):\r\n";
    ss << "  bit 0      Command Format In Spec: " << static_cast<int>(data->NVSCC.CommandFormatInSpec) << "\r\n";
    ss << "  bit 1..7   Reserved: " << static_cast<int>(data->NVSCC.Reserved) << "\r\n";

    ss << "byte 531     M - Namespace Write Protection Capabilities (NWPC):\r\n";
    ss << "  bit 0      Write Protect: " << static_cast<int>(data->NWPC.WriteProtect) << "\r\n";
    ss << "  bit 1      Until Power Cycle: " << static_cast<int>(data->NWPC.UntilPowerCycle) << "\r\n";
    ss << "  bit 2      Permanent: " << static_cast<int>(data->NWPC.Permanent) << "\r\n";
    ss << "  bit 3..7   Reserved: " << static_cast<int>(data->NWPC.Reserved) << "\r\n";

    ss << "byte 532..533 O - Atomic Compare & Write Unit (ACWU): " << data->ACWU << "\r\n";

    ss << "byte 534..535 Reserved4" << "\r\n";

    ss << "byte 536..539 O - SGL Support (SGLS):\r\n";
    ss << "  bit 0..1    SGL Supported: " << static_cast<int>(data->SGLS.SGLSupported) << "\r\n";
    ss << "  bit 2       Keyed SGL Data: " << static_cast<int>(data->SGLS.KeyedSGLData) << "\r\n";
    ss << "  bit 3..15   Reserved0" << "\r\n";
    ss << "  bit 16      Bit Bucket Descriptor Supported: " << static_cast<int>(data->SGLS.BitBucketDescrSupported) << "\r\n";
    ss << "  bit 17      Byte Aligned Contiguous Physical Buffer: " << static_cast<int>(data->SGLS.ByteAlignedContiguousPhysicalBuffer) << "\r\n";
    ss << "  bit 18      SGL Length Larger Than Data Length: " << static_cast<int>(data->SGLS.SGLLengthLargerThanDataLength) << "\r\n";
    ss << "  bit 19      MPTR SGL Descriptor: " << static_cast<int>(data->SGLS.MPTRSGLDescriptor) << "\r\n";
    ss << "  bit 20      Address Field SGL Data Block: " << static_cast<int>(data->SGLS.AddressFieldSGLDataBlock) << "\r\n";
    ss << "  bit 21      Transport SGL Data: " << static_cast<int>(data->SGLS.TransportSGLData) << "\r\n";
    ss << "  bit 22..31  Reserved1: " << static_cast<int>(data->SGLS.Reserved1) << "\r\n";

    ss << "byte 540..543 O - Maximum Number of Allowed Namespace (MNAN): "
        << data->MNAN << "\r\n";

    ss << "byte 544..767 Reserved6: [Reserved]\r\n";

    ss << "byte 768:1023   M - NVM Subsystem NVMe Qualified Name (SUBNQN):\r\n";
    ss << "  Data:\r\n";
    for (size_t i = 0; i < 256; ++i) {
        ss << "  " << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(data->SUBNQN[i]);
        if ((i + 1) % 16 == 0) { // Add a new line every 16 bytes for readability
            ss << "\r\n";
        }
        else {
            ss << " ";
        }
    }
    ss << "\r\n";


    ss << "byte 1024..1791 Reserved7: [Reserved]\r\n";

    ss << "byte 1792..2047 Reserved8: [Refer to NVMe over Fabrics Specification]\r\n";
    ss << "byte 2048..3071 Power State Descriptors\r\n";
    ss << "byte 3072..4095 Vendor Specific\r\n";
    ss << "<<<<<<<<<<<<<<<<<<<<<NVMe Identify End\r\n";
    ss << "\r\n";
}

void DebugPrintIdentifyData(BYTE* identifyData, DWORD dataSize) {
    for (DWORD i = 0; i < dataSize; i++) {
        // 打印字节，以十六进制形式输出，每个字节占两位
        DebugPrint("%02X ", identifyData[i]);

        // 每16个字节后换行，方便阅读
        if ((i + 1) % 16 == 0) {
            DebugPrint("\r\n");
        }
    }

    // 如果最后一行未满16字节，手动换行
    if (dataSize % 16 != 0) {
        DebugPrint("\r\n");
    }
}

__declspec(align(4096)) BYTE g_NVMeIdentifyData[NVME_IDENTIFY_DATA_LENGTH];
__declspec(align(512)) BYTE g_ATAIdentifyData[ATA_IDENTIFY_DATA_LENGTH];

BOOL GetNVMeIdentifyData(HANDLE hDrive, BYTE* identifyData) {
    BOOL result = FALSE;
    PVOID buffer = NULL;
    ULONG bufferLength = 0;
    ULONG returnedLength = 0;

    PSTORAGE_PROPERTY_QUERY query = NULL;
    PSTORAGE_PROTOCOL_SPECIFIC_DATA protocolData = NULL;
    PSTORAGE_PROTOCOL_DATA_DESCRIPTOR protocolDataDescr = NULL;

    // Allocate buffer for query and NVMe data
    bufferLength = FIELD_OFFSET(STORAGE_PROPERTY_QUERY, AdditionalParameters) + sizeof(STORAGE_PROTOCOL_SPECIFIC_DATA) + NVME_MAX_LOG_SIZE;
    buffer = malloc(bufferLength);

    if (buffer == NULL) {
        DebugPrint("GetNVMeIdentifyData: Allocate buffer failed.\r\n");
        return FALSE;
    }

    // Initialize query data structure to get Identify Controller Data
    ZeroMemory(buffer, bufferLength);

    query = (PSTORAGE_PROPERTY_QUERY)buffer;
    protocolDataDescr = (PSTORAGE_PROTOCOL_DATA_DESCRIPTOR)buffer;
    protocolData = (PSTORAGE_PROTOCOL_SPECIFIC_DATA)query->AdditionalParameters;

    query->PropertyId = StorageAdapterProtocolSpecificProperty;
    query->QueryType = PropertyStandardQuery;

    protocolData->ProtocolType = ProtocolTypeNvme;
    protocolData->DataType = NVMeDataTypeIdentify;
    protocolData->ProtocolDataRequestValue = NVME_IDENTIFY_CNS_CONTROLLER;
    protocolData->ProtocolDataRequestSubValue = 0;
    protocolData->ProtocolDataOffset = sizeof(STORAGE_PROTOCOL_SPECIFIC_DATA);
    protocolData->ProtocolDataLength = NVME_MAX_LOG_SIZE;

    // Send request to get Identify Controller Data
    result = DeviceIoControl(
        hDrive,
        IOCTL_STORAGE_QUERY_PROPERTY,
        buffer,
        bufferLength,
        buffer,
        bufferLength,
        &returnedLength,
        NULL
    );

    if (!result) {
        DebugPrint("GetNVMeIdentifyData: DeviceIoControl failed with error %lu.\r\n", GetLastError());
        free(buffer);
        return FALSE;
    }

    // Validate and process the returned data
    ZeroMemory(buffer, bufferLength);
    query = (PSTORAGE_PROPERTY_QUERY)buffer;
    protocolDataDescr = (PSTORAGE_PROTOCOL_DATA_DESCRIPTOR)buffer;
    protocolData = (PSTORAGE_PROTOCOL_SPECIFIC_DATA)query->AdditionalParameters;

    query->PropertyId = StorageDeviceProtocolSpecificProperty;
    query->QueryType = PropertyStandardQuery;

    protocolData->ProtocolType = ProtocolTypeNvme;
    protocolData->DataType = NVMeDataTypeIdentify;
    protocolData->ProtocolDataRequestValue = NVME_IDENTIFY_CNS_CONTROLLER;
    protocolData->ProtocolDataRequestSubValue = 0;
    protocolData->ProtocolDataOffset = sizeof(STORAGE_PROTOCOL_SPECIFIC_DATA);
    protocolData->ProtocolDataLength = NVME_MAX_LOG_SIZE;

    // Send request again to get the actual data
    result = DeviceIoControl(
        hDrive,
        IOCTL_STORAGE_QUERY_PROPERTY,
        buffer,
        bufferLength,
        buffer,
        bufferLength,
        &returnedLength,
        NULL
    );

    if (!result) {
        DebugPrint("GetNVMeIdentifyData: DeviceIoControl failed with error %lu.\r\n", GetLastError());
        free(buffer);
        return FALSE;
    }

    if ((protocolDataDescr->Version != sizeof(STORAGE_PROTOCOL_DATA_DESCRIPTOR)) ||
        (protocolDataDescr->Size != sizeof(STORAGE_PROTOCOL_DATA_DESCRIPTOR))) {
        DebugPrint("GetNVMeIdentifyData: Data descriptor header not valid.\r\n");
        free(buffer);
        return FALSE;
    }

    protocolData = &protocolDataDescr->ProtocolSpecificData;

    if ((protocolData->ProtocolDataOffset < sizeof(STORAGE_PROTOCOL_SPECIFIC_DATA)) ||
        (protocolData->ProtocolDataLength < NVME_MAX_LOG_SIZE)) {
        DebugPrint("GetNVMeIdentifyData: ProtocolData Offset/Length not valid.\r\n");
        free(buffer);
        return FALSE;
    }

    // Copy raw NVMe Identify Controller Data to output buffer
    PCHAR rawData = (PCHAR)protocolData + protocolData->ProtocolDataOffset;

    memcpy(identifyData, rawData, protocolData->ProtocolDataLength);
    DebugPrint("GetNVMeIdentifyData: Identify Controller Data succeeded.\r\n");
    //DebugPrintIdentifyData(identifyData, protocolData->ProtocolDataLength);
    free(buffer);
    return TRUE;
}

std::vector<int> GetAllPhysicalDriveNumbers() {
    std::vector<int> driveNumbers;
    char driveName[256];

    for (int i = 0; i < 32; ++i) {
        sprintf_s(driveName, "\\\\.\\PhysicalDrive%d", i);

        HANDLE hDevice = CreateFileA(
            driveName,
            0,
            FILE_SHARE_READ | FILE_SHARE_WRITE,
            NULL,
            OPEN_EXISTING,
            0,
            NULL
        );

        if (hDevice == INVALID_HANDLE_VALUE) {
            continue; // Skip if the device can't be opened
        }

        STORAGE_DEVICE_NUMBER deviceNumber;
        DWORD bytesReturned;

        if (DeviceIoControl(
            hDevice,
            IOCTL_STORAGE_GET_DEVICE_NUMBER,
            NULL,
            0,
            &deviceNumber,
            sizeof(deviceNumber),
            &bytesReturned,
            NULL
        )) {
            driveNumbers.push_back(deviceNumber.DeviceNumber);
        }

        CloseHandle(hDevice);
    }

    return driveNumbers;
}

HANDLE OpenPhysicalDrive(int driveNumber) {
    char driveName[256];
    sprintf_s(driveName, "\\\\.\\PhysicalDrive%d", driveNumber);

    HANDLE hDrive = CreateFileA(
        driveName,
        GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL,
        OPEN_EXISTING,
        0,
        NULL
    );
    if (hDrive == INVALID_HANDLE_VALUE) {
        DebugPrint("Failed to open drive: %s\r\n", driveName);
        // Optional: Get more error details
        DWORD dwError = GetLastError();
        DebugPrint("Error code: %lu\r\n", dwError);
    }
    else
    {
        DebugPrint("Ok to open drive: %s\r\n", driveName);
    }

    return hDrive;
}

// USB bridge is an USB device, so we need to use SCSI command set to communicate with it
BOOL SCSIGetATAIdentifyData(HANDLE hDrive, BYTE* identifyData) {
    SCSI_PASS_THROUGH_WITH_BUFFERS sptwb;
    DWORD bytesReturned;

    if (hDrive == INVALID_HANDLE_VALUE || identifyData == NULL) {
        return FALSE; // Invalid parameters
    }

    // Zero the structure
    ZeroMemory(&sptwb, sizeof(SCSI_PASS_THROUGH_WITH_BUFFERS));

    // Initialize SCSI_PASS_THROUGH structure
    sptwb.spt.Length = sizeof(SCSI_PASS_THROUGH);
    sptwb.spt.CdbLength = 12; // ATA PASS-THROUGH (12) Command Descriptor Block size
    sptwb.spt.SenseInfoLength = sizeof(sptwb.ucSenseBuf);
    sptwb.spt.DataIn = SCSI_IOCTL_DATA_IN;
    sptwb.spt.DataTransferLength = 512; // ATA IDENTIFY data size
    sptwb.spt.TimeOutValue = 10; // Timeout in seconds
    sptwb.spt.DataBufferOffset = offsetof(SCSI_PASS_THROUGH_WITH_BUFFERS, ucDataBuf);
    sptwb.spt.SenseInfoOffset = offsetof(SCSI_PASS_THROUGH_WITH_BUFFERS, ucSenseBuf);

    // ATA PASS-THROUGH (12) CDB (Command Descriptor Block)
    sptwb.spt.Cdb[0] = 0xA1;  // ATA PASS-THROUGH (12)
    sptwb.spt.Cdb[1] = 0x08;  // PIO Data-In
    sptwb.spt.Cdb[2] = 0x2E;  // Set to bypass queue, 48-bit LBA
    sptwb.spt.Cdb[3] = 0;     // Reserved
    sptwb.spt.Cdb[4] = 0;     // Reserved
    sptwb.spt.Cdb[5] = 0;     // Reserved
    sptwb.spt.Cdb[6] = 0;     // Reserved
    sptwb.spt.Cdb[7] = 0;     // Reserved
    sptwb.spt.Cdb[8] = 0;     // Reserved
    sptwb.spt.Cdb[9] = ATA_IDENTIFY;  // ATA IDENTIFY DEVICE command
    sptwb.spt.Cdb[10] = 0;    // Reserved
    sptwb.spt.Cdb[11] = 0;    // Reserved

    // Send the command to the drive
    BOOL result = DeviceIoControl(
        hDrive,                          // Device handle
        IOCTL_SCSI_PASS_THROUGH,         // Control code
        &sptwb,                          // Input buffer
        sizeof(SCSI_PASS_THROUGH_WITH_BUFFERS), // Size of the input buffer
        &sptwb,                          // Output buffer (same as input here)
        sizeof(SCSI_PASS_THROUGH_WITH_BUFFERS), // Size of the output buffer
        &bytesReturned,                  // Number of bytes returned
        NULL                             // OVERLAPPED structure
    );

    if (result) {
        // Copy the IDENTIFY DEVICE data to the output buffer
        memcpy(identifyData, sptwb.ucDataBuf, 512);
        DebugPrintIdentifyData(identifyData, 512);
    }
    else {
        DebugPrint("Error retrieving ATA identify data: %d\r\n", GetLastError());
    }

    return result;
}

void SwapAndFormatATAString(const UCHAR* src, char* dest, size_t length) {
    // Each word contains two characters (2 bytes), so we swap every two bytes.
    for (size_t i = 0; i < length; i += 2) {
        if (i + 1 < length) {
            dest[i] = src[i + 1];
            dest[i + 1] = src[i];
        }
        else {
            // For odd length, handle the last byte as is
            dest[i] = src[i];
        }
    }
    dest[length] = '\0';  // Null-terminate the string
}

void FormatATAIdentifyField(const IDENTIFY_DEVICE_DATA* data) {
    ss << ">>>>>>>>>>>>>>>>>>>>>ATA Identify Start\r\n";
    // General Configuration (word 0)
    ss << "word 0        General Configuration:\r\n";
    ss << "  bit 0        Reserved1: " << static_cast<int>(data->GeneralConfiguration.Reserved1) << "\r\n";
    ss << "  bit 1        Retired3: " << static_cast<int>(data->GeneralConfiguration.Retired3) << "\r\n";
    ss << "  bit 2        Response Incomplete: " << static_cast<int>(data->GeneralConfiguration.ResponseIncomplete) << "\r\n";
    ss << "  bit 3-5      Retired2: " << static_cast<int>(data->GeneralConfiguration.Retired2) << "\r\n";
    ss << "  bit 6        Fixed Device: " << static_cast<int>(data->GeneralConfiguration.FixedDevice) << "\r\n";
    ss << "  bit 7        Removable Media: " << static_cast<int>(data->GeneralConfiguration.RemovableMedia) << "\r\n";
    ss << "  bit 8-14     Retired1: " << static_cast<int>(data->GeneralConfiguration.Retired1) << "\r\n";
    ss << "  bit 15       Device Type: " << static_cast<int>(data->GeneralConfiguration.DeviceType) << "\r\n";

    // Number of Cylinders (word 1)
    ss << "word 1        Number of Cylinders: " << data->NumCylinders << "\r\n";

    // Specific Configuration (word 2)
    ss << "word 2        Specific Configuration: 0x" << std::hex << std::setw(4) << std::setfill('0') << data->SpecificConfiguration << std::dec << "\r\n";

    // Number of Heads (word 3)
    ss << "word 3        Number of Heads: " << data->NumHeads << "\r\n";
    // Retired1 (word 4-5)
    ss << "word 4-5      Retired1: 0x" << std::hex << std::setw(4) << std::setfill('0') << data->Retired1[0]
        << " 0x" << std::setw(4) << std::setfill('0') << data->Retired1[1] << std::dec << "\r\n";

    // Number of Sectors Per Track (word 6)
    ss << "word 6        Number of Sectors Per Track: " << data->NumSectorsPerTrack << "\r\n";

    // Vendor Unique 1 (word 7-9)
    ss << "word 7-9      Vendor Unique 1: 0x" << std::hex << std::setw(4) << std::setfill('0') << data->VendorUnique1[0]
        << " 0x" << std::setw(4) << std::setfill('0') << data->VendorUnique1[1]
        << " 0x" << std::setw(4) << std::setfill('0') << data->VendorUnique1[2] << std::dec << "\r\n";

    // Serial Number (word 10-19), apply byte swapping
    char snRev[21]; // 20 bytes + null terminator
    SwapAndFormatATAString(data->SerialNumber, snRev, 20);
    ss << "word 23-26    Serial Number: " << snRev << "\r\n";

    // Retired2 (word 20-21)
    ss << "word 20-21    Retired2: 0x" << std::hex << std::setw(4) << std::setfill('0') << data->Retired2[0]
        << " 0x" << std::setw(4) << std::setfill('0') << data->Retired2[1] << std::dec << "\r\n";

    // Obsolete1 (word 22)
    ss << "word 22       Obsolete1: 0x" << std::hex << std::setw(4) << std::setfill('0') << data->Obsolete1 << std::dec << "\r\n";

    // Firmware Revision (word 23-26), apply byte swapping
    char firmwareRev[9]; // 8 bytes + null terminator
    SwapAndFormatATAString(data->FirmwareRevision, firmwareRev, 8);
    ss << "word 23-26    Firmware Revision: " << firmwareRev << "\r\n";

    // Model Number (word 27-46), apply byte swapping
    char modelNumber[41]; // 40 bytes + null terminator
    SwapAndFormatATAString(data->ModelNumber, modelNumber, 40);
    ss << "word 27-46    Model Number: " << modelNumber << "\r\n";

    // Maximum Block Transfer (word 47, low byte)
    ss << "word 47 (low) Maximum Block Transfer: " << static_cast<int>(data->MaximumBlockTransfer) << "\r\n";

    // Vendor Unique 2 (word 47, high byte)
    ss << "word 47 (high) Vendor Unique 2: " << static_cast<int>(data->VendorUnique2) << "\r\n";

    // Trusted Computing (word 48)
    ss << "word 48       Trusted Computing:\r\n";
    ss << "  bit 0        Feature Supported: " << static_cast<int>(data->TrustedComputing.FeatureSupported) << "\r\n";
    ss << "  bit 1-15     Reserved: " << static_cast<int>(data->TrustedComputing.Reserved) << "\r\n";

    // Capabilities (word 49-50)
    ss << "word 49       Capabilities:\r\n";
    ss << "  bits 0-1     Current Long Physical Sector Alignment: " << static_cast<int>(data->Capabilities.CurrentLongPhysicalSectorAlignment) << "\r\n";
    ss << "  bits 2-7     Reserved Byte 49: " << static_cast<int>(data->Capabilities.ReservedByte49) << "\r\n";
    ss << "  bit 8        DMA Supported: " << static_cast<int>(data->Capabilities.DmaSupported) << "\r\n";
    ss << "  bit 9        LBA Supported: " << static_cast<int>(data->Capabilities.LbaSupported) << "\r\n";
    ss << "  bit 10       IORDY Disable: " << static_cast<int>(data->Capabilities.IordyDisable) << "\r\n";
    ss << "  bit 11       IORDY Supported: " << static_cast<int>(data->Capabilities.IordySupported) << "\r\n";
    ss << "  bit 12       Reserved: " << static_cast<int>(data->Capabilities.Reserved1) << "\r\n";
    ss << "  bit 13       Standby Timer Support: " << static_cast<int>(data->Capabilities.StandybyTimerSupport) << "\r\n";
    ss << "  bits 14-15   Reserved: " << static_cast<int>(data->Capabilities.Reserved2) << "\r\n";

    // Capabilities Reserved Word 50 (word 50)
    ss << "word 50       Capabilities Reserved Word 50: 0x" << std::hex << std::setw(4) << std::setfill('0') << data->Capabilities.ReservedWord50 << std::dec << "\r\n";

        // ObsoleteWords51 (words 51-52)
    ss << "words 51-52   ObsoleteWords51: 0x" << std::hex << std::setw(4) << std::setfill('0') << data->ObsoleteWords51[0] << " 0x" << data->ObsoleteWords51[1] << std::dec << "\r\n";

    // TranslationFieldsValid (word 53)
    ss << "word 53       Translation Fields Valid:\r\n";
    ss << "  bits 0-2     Translation Fields Valid: " << static_cast<int>(data->TranslationFieldsValid) << "\r\n";
    ss << "  bits 3-7     Reserved: " << static_cast<int>(data->Reserved3) << "\r\n";
    ss << "  bits 8-15    Free-Fall Control Sensitivity: " << static_cast<int>(data->FreeFallControlSensitivity) << "\r\n";

    // NumberOfCurrentCylinders (word 54)
    ss << "word 54..58   Obsolete" << "\r\n";

    // CurrentMultiSectorSetting (word 59, byte 0)
    ss << "word 59, byte 0 Current Multi-Sector Setting: " << static_cast<int>(data->CurrentMultiSectorSetting) << "\r\n";

    // MultiSectorSettingValid (word 59, byte 1, bit 0)
    ss << "word 59, byte 1 bit 0  Multi-Sector Setting Valid: " << static_cast<int>(data->MultiSectorSettingValid) << "\r\n";

    // ReservedByte59 (word 59, byte 1, bits 1-3)
    ss << "word 59, byte 1 bits 1-3 Reserved: " << static_cast<int>(data->ReservedByte59) << "\r\n";

    // SanitizeFeatureSupported (word 59, byte 1, bit 4)
    ss << "word 59, byte 1 bit 4  Sanitize Feature Supported: " << static_cast<int>(data->SanitizeFeatureSupported) << "\r\n";

    // CryptoScrambleExtCommandSupported (word 59, byte 1, bit 5)
    ss << "word 59, byte 1 bit 5  Crypto Scramble Ext Command Supported: " << static_cast<int>(data->CryptoScrambleExtCommandSupported) << "\r\n";

    // OverwriteExtCommandSupported (word 59, byte 1, bit 6)
    ss << "word 59, byte 1 bit 6  Overwrite Ext Command Supported: " << static_cast<int>(data->OverwriteExtCommandSupported) << "\r\n";

    // BlockEraseExtCommandSupported (word 59, byte 1, bit 7)
    ss << "word 59, byte 1 bit 7  Block Erase Ext Command Supported: " << static_cast<int>(data->BlockEraseExtCommandSupported) << "\r\n";

    // UserAddressableSectors (words 60-61)
    ss << "words 60-61   User Addressable Sectors: " << data->UserAddressableSectors << "\r\n";

    // ObsoleteWord62 (word 62)
    ss << "word 62       Obsolete Word 62: 0x" << std::hex << std::setw(4) << std::setfill('0') << data->ObsoleteWord62 << std::dec << "\r\n";

    // MultiWordDMASupport (word 63, byte 0)
    ss << "word 63, byte 0 Multi-Word DMA Support: " << static_cast<int>(data->MultiWordDMASupport) << "\r\n";

    // MultiWordDMAActive (word 63, byte 1)
    ss << "word 63, byte 1 Multi-Word DMA Active: " << static_cast<int>(data->MultiWordDMAActive) << "\r\n";

    // AdvancedPIOModes (word 64, byte 0)
    ss << "word 64, byte 0 Advanced PIO Modes: " << static_cast<int>(data->AdvancedPIOModes) << "\r\n";

    // ReservedByte64 (word 64, byte 1)
    ss << "word 64, byte 1 Reserved Byte 64: " << static_cast<int>(data->ReservedByte64) << "\r\n";

    // MinimumMWXferCycleTime (word 65)
    ss << "word 65       Minimum Multi-Word DMA Transfer Cycle Time: " << data->MinimumMWXferCycleTime << "\r\n";

    // RecommendedMWXferCycleTime (word 66)
    ss << "word 66       Recommended Multi-Word DMA Transfer Cycle Time: " << data->RecommendedMWXferCycleTime << "\r\n";

    // MinimumPIOCycleTime (word 67)
    ss << "word 67       Minimum PIO Cycle Time Without IORDY: " << data->MinimumPIOCycleTime << "\r\n";

    // MinimumPIOCycleTimeIORDY (word 68)
    ss << "word 68       Minimum PIO Cycle Time With IORDY: " << data->MinimumPIOCycleTimeIORDY << "\r\n";

    ss << "word 69      bits 1..0    Zoned Capabilities: " << static_cast<int>(data->AdditionalSupported.ZonedCapabilities) << "\r\n";
    ss << "word 69      bits 2       Non-Volatile Write Cache: " << static_cast<int>(data->AdditionalSupported.NonVolatileWriteCache) << "\r\n";
    ss << "word 69      bits 3       Extended User Addressable Sectors Supported: " << static_cast<int>(data->AdditionalSupported.ExtendedUserAddressableSectorsSupported) << "\r\n";
    ss << "word 69      bits 4       Device Encrypts All User Data: " << static_cast<int>(data->AdditionalSupported.DeviceEncryptsAllUserData) << "\r\n";
    ss << "word 69      bits 5       Read Zero After Trim Supported: " << static_cast<int>(data->AdditionalSupported.ReadZeroAfterTrimSupported) << "\r\n";
    ss << "word 69      bits 6       Optional 28-bit Commands Supported: " << static_cast<int>(data->AdditionalSupported.Optional28BitCommandsSupported) << "\r\n";
    ss << "word 69      bits 7       IEEE 1667: " << static_cast<int>(data->AdditionalSupported.IEEE1667) << "\r\n";
    ss << "word 69      bits 8       Download Microcode DMA Supported: " << static_cast<int>(data->AdditionalSupported.DownloadMicrocodeDmaSupported) << "\r\n";
    ss << "word 69      bits 9       SetMaxSetPassword Unlock DMA Supported: " << static_cast<int>(data->AdditionalSupported.SetMaxSetPasswordUnlockDmaSupported) << "\r\n";
    ss << "word 69      bits 10      Write Buffer DMA Supported: " << static_cast<int>(data->AdditionalSupported.WriteBufferDmaSupported) << "\r\n";
    ss << "word 69      bits 11      Read Buffer DMA Supported: " << static_cast<int>(data->AdditionalSupported.ReadBufferDmaSupported) << "\r\n";
    ss << "word 69      bits 12      Device Config Identify Set DMA Supported: " << static_cast<int>(data->AdditionalSupported.DeviceConfigIdentifySetDmaSupported) << "\r\n";
    ss << "word 69      bits 13      LPSA ERC Supported: " << static_cast<int>(data->AdditionalSupported.LPSAERCSupported) << "\r\n";
    ss << "word 69      bits 14      Deterministic Read After Trim Supported: " << static_cast<int>(data->AdditionalSupported.DeterministicReadAfterTrimSupported) << "\r\n";
    ss << "word 69      bits 15      CFast Spec Supported: " << static_cast<int>(data->AdditionalSupported.CFastSpecSupported) << "\r\n";

    // Example for ReservedWords70
    ss << "word 70-74   Reserved Words: byte reserved\r\n";

    // Example for QueueDepth
    ss << "word 75      bits 4..0    Queue Depth: " << static_cast<int>(data->QueueDepth) << "\r\n";
    ss << "word 75      bits 15..5   Reserved: byte reserved\r\n";

    ss << "word 76      bit  0       Reserved0: byte reserved\r\n";
    ss << "word 76      bit  1       SATA Gen1: " << static_cast<int>(data->SerialAtaCapabilities.SataGen1) << "\r\n";
    ss << "word 76      bit  2       SATA Gen2: " << static_cast<int>(data->SerialAtaCapabilities.SataGen2) << "\r\n";
    ss << "word 76      bit  3       SATA Gen3: " << static_cast<int>(data->SerialAtaCapabilities.SataGen3) << "\r\n";
    ss << "word 76      bits 7..4    Reserved1: byte reserved\r\n";
    ss << "word 76      bit  8       NCQ Supported: " << static_cast<int>(data->SerialAtaCapabilities.NCQ) << "\r\n";
    ss << "word 76      bit  9       HIPM Supported: " << static_cast<int>(data->SerialAtaCapabilities.HIPM) << "\r\n";
    ss << "word 76      bit  10      PHY Events Supported: " << static_cast<int>(data->SerialAtaCapabilities.PhyEvents) << "\r\n";
    ss << "word 76      bit  11      NCQ Unload Supported: " << static_cast<int>(data->SerialAtaCapabilities.NcqUnload) << "\r\n";
    ss << "word 76      bit  12      NCQ Priority Supported: " << static_cast<int>(data->SerialAtaCapabilities.NcqPriority) << "\r\n";
    ss << "word 76      bit  13      Host Auto PS: " << static_cast<int>(data->SerialAtaCapabilities.HostAutoPS) << "\r\n";
    ss << "word 76      bit  14      Device Auto PS: " << static_cast<int>(data->SerialAtaCapabilities.DeviceAutoPS) << "\r\n";
    ss << "word 76      bit  15      Read Log DMA Supported: " << static_cast<int>(data->SerialAtaCapabilities.ReadLogDMA) << "\r\n";

    // The next word (77)
    ss << "word 77      bit  0       Reserved2: byte reserved\r\n";
    ss << "word 77      bits 2..0    Current Speed: " << static_cast<int>(data->SerialAtaCapabilities.CurrentSpeed) << "\r\n";
    ss << "word 77      bit  3       NCQ Streaming Supported: " << static_cast<int>(data->SerialAtaCapabilities.NcqStreaming) << "\r\n";
    ss << "word 77      bit  4       NCQ Queue Management Supported: " << static_cast<int>(data->SerialAtaCapabilities.NcqQueueMgmt) << "\r\n";
    ss << "word 77      bit  5       NCQ Receive & Send Supported: " << static_cast<int>(data->SerialAtaCapabilities.NcqReceiveSend) << "\r\n";
    ss << "word 77      bit  6       DEVSLP to Reduced Power State: " << static_cast<int>(data->SerialAtaCapabilities.DEVSLPtoReducedPwrState) << "\r\n";
    ss << "word 77      bits 15..7   Reserved3: byte reserved\r\n";

    // SerialAtaFeaturesSupported (word 78)
    ss << "word 78      bit  0       Reserved0: byte reserved\r\n";
    ss << "word 78      bit  1       Non-Zero Offsets Supported: " << static_cast<int>(data->SerialAtaFeaturesSupported.NonZeroOffsets) << "\r\n";
    ss << "word 78      bit  2       DMA Setup Auto-Activate Supported: " << static_cast<int>(data->SerialAtaFeaturesSupported.DmaSetupAutoActivate) << "\r\n";
    ss << "word 78      bit  3       DIPM Supported: " << static_cast<int>(data->SerialAtaFeaturesSupported.DIPM) << "\r\n";
    ss << "word 78      bit  4       In-Order Data Supported: " << static_cast<int>(data->SerialAtaFeaturesSupported.InOrderData) << "\r\n";
    ss << "word 78      bit  5       Hardware Feature Control Supported: " << static_cast<int>(data->SerialAtaFeaturesSupported.HardwareFeatureControl) << "\r\n";
    ss << "word 78      bit  6       Software Settings Preservation Supported: " << static_cast<int>(data->SerialAtaFeaturesSupported.SoftwareSettingsPreservation) << "\r\n";
    ss << "word 78      bit  7       NCQ Autosense Supported: " << static_cast<int>(data->SerialAtaFeaturesSupported.NCQAutosense) << "\r\n";
    ss << "word 78      bit  8       DEVSLP Supported: " << static_cast<int>(data->SerialAtaFeaturesSupported.DEVSLP) << "\r\n";
    ss << "word 78      bit  9       Hybrid Information Supported: " << static_cast<int>(data->SerialAtaFeaturesSupported.HybridInformation) << "\r\n";
    ss << "word 78      bits 15..10  Reserved1: byte reserved\r\n";

    // SerialAtaFeaturesEnabled (word 79)
    ss << "word 79      bit  0       Reserved0: byte reserved\r\n";
    ss << "word 79      bit  1       Non-Zero Offsets Enabled: " << static_cast<int>(data->SerialAtaFeaturesEnabled.NonZeroOffsets) << "\r\n";
    ss << "word 79      bit  2       DMA Setup Auto-Activate Enabled: " << static_cast<int>(data->SerialAtaFeaturesEnabled.DmaSetupAutoActivate) << "\r\n";
    ss << "word 79      bit  3       DIPM Enabled: " << static_cast<int>(data->SerialAtaFeaturesEnabled.DIPM) << "\r\n";
    ss << "word 79      bit  4       In-Order Data Enabled: " << static_cast<int>(data->SerialAtaFeaturesEnabled.InOrderData) << "\r\n";
    ss << "word 79      bit  5       Hardware Feature Control Enabled: " << static_cast<int>(data->SerialAtaFeaturesEnabled.HardwareFeatureControl) << "\r\n";
    ss << "word 79      bit  6       Software Settings Preservation Enabled: " << static_cast<int>(data->SerialAtaFeaturesEnabled.SoftwareSettingsPreservation) << "\r\n";
    ss << "word 79      bit  7       Device Auto PS Enabled: " << static_cast<int>(data->SerialAtaFeaturesEnabled.DeviceAutoPS) << "\r\n";
    ss << "word 79      bit  8       DEVSLP Enabled: " << static_cast<int>(data->SerialAtaFeaturesEnabled.DEVSLP) << "\r\n";
    ss << "word 79      bit  9       Hybrid Information Enabled: " << static_cast<int>(data->SerialAtaFeaturesEnabled.HybridInformation) << "\r\n";
    ss << "word 79      bits 15..10  Reserved1: byte reserved\r\n";

    // Major and Minor Revisions (word 80)
    ss << "word 80      Major Revision: " << static_cast<int>(data->MajorRevision) << "\r\n";
    ss << "word 81      Minor Revision: " << static_cast<int>(data->MinorRevision) << "\r\n";

    ss << "Command Set Support:\r\n";

    ss << "word 82      bit 0      SMART Commands Supported: " << static_cast<int>(data->CommandSetSupport.SmartCommands) << "\r\n";
    ss << "word 82      bit 1      Security Mode Supported: " << static_cast<int>(data->CommandSetSupport.SecurityMode) << "\r\n";
    ss << "word 82      bit 2      Removable Media Feature Supported: " << static_cast<int>(data->CommandSetSupport.RemovableMediaFeature) << "\r\n";
    ss << "word 82      bit 3      Power Management Supported: " << static_cast<int>(data->CommandSetSupport.PowerManagement) << "\r\n";
    ss << "word 82      bit 4      Reserved: byte reserved\r\n";
    ss << "word 82      bit 5      Write Cache Supported: " << static_cast<int>(data->CommandSetSupport.WriteCache) << "\r\n";
    ss << "word 82      bit 6      Look-Ahead Supported: " << static_cast<int>(data->CommandSetSupport.LookAhead) << "\r\n";
    ss << "word 82      bit 7      Release Interrupt Supported: " << static_cast<int>(data->CommandSetSupport.ReleaseInterrupt) << "\r\n";
    ss << "word 82      bit 8      Service Interrupt Supported: " << static_cast<int>(data->CommandSetSupport.ServiceInterrupt) << "\r\n";
    ss << "word 82      bit 9      Device Reset Supported: " << static_cast<int>(data->CommandSetSupport.DeviceReset) << "\r\n";
    ss << "word 82      bit 10     Host Protected Area Supported: " << static_cast<int>(data->CommandSetSupport.HostProtectedArea) << "\r\n";
    ss << "word 82      bit 11     Obsolete: byte obsolete\r\n";
    ss << "word 82      bit 12     Write Buffer Supported: " << static_cast<int>(data->CommandSetSupport.WriteBuffer) << "\r\n";
    ss << "word 82      bit 13     Read Buffer Supported: " << static_cast<int>(data->CommandSetSupport.ReadBuffer) << "\r\n";
    ss << "word 82      bit 14     NOP Supported: " << static_cast<int>(data->CommandSetSupport.Nop) << "\r\n";
    ss << "word 82      bit 15     Obsolete: byte obsolete\r\n";

    ss << "word 83      bit 0      Download Microcode Supported: " << static_cast<int>(data->CommandSetSupport.DownloadMicrocode) << "\r\n";
    ss << "word 83      bit 1      DMA Queued Supported: " << static_cast<int>(data->CommandSetSupport.DmaQueued) << "\r\n";
    ss << "word 83      bit 2      CFA Supported: " << static_cast<int>(data->CommandSetSupport.Cfa) << "\r\n";
    ss << "word 83      bit 3      Advanced Power Management Supported: " << static_cast<int>(data->CommandSetSupport.AdvancedPm) << "\r\n";
    ss << "word 83      bit 4      MSN Supported: " << static_cast<int>(data->CommandSetSupport.Msn) << "\r\n";
    ss << "word 83      bit 5      Power Up In Standby Supported: " << static_cast<int>(data->CommandSetSupport.PowerUpInStandby) << "\r\n";
    ss << "word 83      bit 6      Manual Power Up Supported: " << static_cast<int>(data->CommandSetSupport.ManualPowerUp) << "\r\n";
    ss << "word 83      bit 7      Reserved: byte reserved\r\n";
    ss << "word 83      bit 8      SET MAX Supported: " << static_cast<int>(data->CommandSetSupport.SetMax) << "\r\n";
    ss << "word 83      bit 9      Acoustics Supported: " << static_cast<int>(data->CommandSetSupport.Acoustics) << "\r\n";
    ss << "word 83      bit 10     Big LBA Supported: " << static_cast<int>(data->CommandSetSupport.BigLba) << "\r\n";
    ss << "word 83      bit 11     Device Configuration Overlay Supported: " << static_cast<int>(data->CommandSetSupport.DeviceConfigOverlay) << "\r\n";
    ss << "word 83      bit 12     Flush Cache Supported: " << static_cast<int>(data->CommandSetSupport.FlushCache) << "\r\n";
    ss << "word 83      bit 13     Flush Cache EXT Supported: " << static_cast<int>(data->CommandSetSupport.FlushCacheExt) << "\r\n";
    ss << "word 83      bit 14     Reserved: byte reserved\r\n";
    ss << "word 83      bit 15     Word 83 Valid: " << static_cast<int>(data->CommandSetSupport.WordValid83) << "\r\n";

    ss << "word 84      bit 0      SMART Error Log Supported: " << static_cast<int>(data->CommandSetSupport.SmartErrorLog) << "\r\n";
    ss << "word 84      bit 1      SMART Self-Test Supported: " << static_cast<int>(data->CommandSetSupport.SmartSelfTest) << "\r\n";
    ss << "word 84      bit 2      Media Serial Number Supported: " << static_cast<int>(data->CommandSetSupport.MediaSerialNumber) << "\r\n";
    ss << "word 84      bit 3      Media Card Pass-Through Supported: " << static_cast<int>(data->CommandSetSupport.MediaCardPassThrough) << "\r\n";
    ss << "word 84      bit 4      Streaming Feature Supported: " << static_cast<int>(data->CommandSetSupport.StreamingFeature) << "\r\n";
    ss << "word 84      bit 5      GP Logging Supported: " << static_cast<int>(data->CommandSetSupport.GpLogging) << "\r\n";
    ss << "word 84      bit 6      Write FUA Supported: " << static_cast<int>(data->CommandSetSupport.WriteFua) << "\r\n";
    ss << "word 84      bit 7      Write Queued FUA Supported: " << static_cast<int>(data->CommandSetSupport.WriteQueuedFua) << "\r\n";
    ss << "word 84      bit 8      WWN 64-Bit Supported: " << static_cast<int>(data->CommandSetSupport.WWN64Bit) << "\r\n";
    ss << "word 84      bit 9      URG Read Stream Supported: " << static_cast<int>(data->CommandSetSupport.URGReadStream) << "\r\n";
    ss << "word 84      bit 10     URG Write Stream Supported: " << static_cast<int>(data->CommandSetSupport.URGWriteStream) << "\r\n";
    ss << "word 84      bit 11     Reserved for Technical Report: byte reserved\r\n";
    ss << "word 84      bit 12     Idle With Unload Feature Supported: " << static_cast<int>(data->CommandSetSupport.IdleWithUnloadFeature) << "\r\n";
    ss << "word 84      bit 13-14  Reserved: byte reserved\r\n";
    ss << "word 84      bit 15     Word 84 Valid: " << static_cast<int>(data->CommandSetSupport.WordValid) << "\r\n";

    // Continue with word 84 and beyond using the same format.


}

enum DriveType {
    DriveTypeUnknown,
    DriveTypeSATA,
    DriveTypeNVMe,
    DriveTypeUSB,
};

DriveType GetDriveType(int driveNumber) {
    std::wstring physicalDrivePath = L"\\\\.\\PhysicalDrive" + std::to_wstring(driveNumber);
    HANDLE hDrive = CreateFileW(
        physicalDrivePath.c_str(),
        GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL,
        OPEN_EXISTING,
        0,
        NULL
    );

    if (hDrive == INVALID_HANDLE_VALUE) {
        DebugPrint("fail to open drive %d\r\n", driveNumber);
        return DriveTypeUnknown;
    }

    STORAGE_PROPERTY_QUERY query;
    ZeroMemory(&query, sizeof(query));
    query.PropertyId = StorageDeviceProperty;
    query.QueryType = PropertyStandardQuery;

    BYTE buffer[1024];
    DWORD bytesReturned = 0;
    if (!DeviceIoControl(
        hDrive,
        IOCTL_STORAGE_QUERY_PROPERTY,
        &query,
        sizeof(query),
        &buffer,
        sizeof(buffer),
        &bytesReturned,
        NULL)) {
        DebugPrint("Failed to query storage properties for drive\r\n");
        CloseHandle(hDrive);
        return DriveTypeUnknown;
    }

    STORAGE_DEVICE_DESCRIPTOR* deviceDescriptor = (STORAGE_DEVICE_DESCRIPTOR*)buffer;
    DriveType driveType = DriveTypeUnknown;

    switch (deviceDescriptor->BusType) {
    case BusTypeAta:
        return DriveTypeSATA;
        break;
    case BusTypeNvme:
        return DriveTypeNVMe;
        break;
    case BusTypeUsb:
        return DriveTypeUSB;
    default:
        break;
    }

    DebugPrint("Drive type %d\r\n", deviceDescriptor->BusType);

    CloseHandle(hDrive);
    return driveType;
}

#define ID_EDIT_COPY 1
#define ID_HELP_ABOUT 2

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    static HWND hEdit;
    static HFONT hFont;

    switch (uMsg) {
    case WM_CREATE: {
        hEdit = CreateWindowEx(
            0,
            L"EDIT",
            NULL,
            WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY,
            0, 0, 0, 0,
            hwnd,
            NULL,
            (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
            NULL
        );
        if (hEdit == NULL) {
            MessageBox(hwnd, L"Failed to create text edit control.", L"Error", MB_OK | MB_ICONERROR);
            return -1;
        }

        hFont = CreateFont(
            16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET,
            OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
            FIXED_PITCH, TEXT("Consolas")
        );
        SendMessage(hEdit, WM_SETFONT, (WPARAM)hFont, TRUE);

        // Create and set up the menu
        HMENU hMenu = CreateMenu();
        HMENU hEditMenu = CreatePopupMenu();
        HMENU hHelpMenu = CreatePopupMenu();

        AppendMenu(hEditMenu, MF_STRING, ID_EDIT_COPY, L"&Copy\tCtrl+C");
        AppendMenu(hHelpMenu, MF_STRING, ID_HELP_ABOUT, L"&About");

        AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hEditMenu, L"&Edit");
        AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hHelpMenu, L"&Help");
        SetMenu(hwnd, hMenu);

        // Get all physical drive numbers
        std::vector<int> driveNumbers = GetAllPhysicalDriveNumbers();
        for (int driveNumber : driveNumbers) {
            DriveType driveType = GetDriveType(driveNumber);
            HANDLE hDrive = OpenPhysicalDrive(driveNumber);
            if (hDrive == INVALID_HANDLE_VALUE) {
                DebugPrint("Drvie %d failed to open.\r\n", driveNumber);
                continue;
            }

            if (driveType == DriveTypeNVMe) {
                if (GetNVMeIdentifyData(hDrive, g_NVMeIdentifyData)) {
                    ss << ">>>>Physical Drive Number: " << driveNumber << "\r\n";
                    FormatNvmeIdentifyField((NVME_IDENTIFY_CONTROLLER_DATA*)g_NVMeIdentifyData);
                }
                else {
                    DebugPrint("Drvie %d: failed to retrieve NUMe Identify data.\r\n", driveNumber);
                }
            }
            else if (driveType == DriveTypeUSB) {
                if (SCSIGetATAIdentifyData(hDrive, g_ATAIdentifyData)) {
                    // to check whether ata device here
                    if (((USHORT* )g_ATAIdentifyData)[0] != 0xFFFF && ((USHORT *)g_ATAIdentifyData)[0] != 0x0000) {
                        // Valid ATA device identified
                        FormatATAIdentifyField((IDENTIFY_DEVICE_DATA*)g_ATAIdentifyData);
                    }
                    else {
                        DebugPrint("Drive %d: not ATA device\r\n", driveNumber);
                    }
                }
                else
                {
                    DebugPrint("Drive %d: failed to get scsi ata identify data\r\n", driveNumber);
                }
            }
            CloseHandle(hDrive);
        }
        // Convert the stringstream to a string and display it in the hEdit control
        std::string outputStr = ss.str();
        SetWindowTextA(hEdit, outputStr.c_str());
        break;
    }
    case WM_COMMAND: {
        switch (LOWORD(wParam)) {
        case ID_EDIT_COPY: {
            int length = GetWindowTextLength(hEdit);
            if (length > 0) {
                HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, (length + 1) * sizeof(TCHAR));
                if (hGlobal) {
                    LPTSTR pText = (LPTSTR)GlobalLock(hGlobal);
                    if (pText) {
                        GetWindowText(hEdit, pText, length + 1);
                        GlobalUnlock(hGlobal);

                        if (OpenClipboard(hwnd)) {
                            EmptyClipboard();
                            SetClipboardData(CF_UNICODETEXT, hGlobal);
                            CloseClipboard();
                        }
                    }
                }
            }
            break;
        }
        case ID_HELP_ABOUT: {
            MessageBox(hwnd,
                L"Software Version: 1.0.0\nAuthor: Li Mingchao",
                L"About",
                MB_OK | MB_ICONINFORMATION);
            break;
        }
        }
        break;
    }
    case WM_SIZE: {
        RECT clientRect;
        GetClientRect(hwnd, &clientRect);
        SetWindowPos(hEdit, NULL,
            10, 10,
            clientRect.right - 20, clientRect.bottom - 20,
            SWP_NOZORDER | SWP_NOACTIVATE);
        break;
    }
    case WM_DESTROY: {
        PostQuitMessage(0);
        break;
    }
    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}

// WinMain：应用程序入口点
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    const wchar_t CLASS_NAME[] = L"SSDInfoWindow";

    WNDCLASS wc = { };
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(
        0,
        CLASS_NAME,
        L"SSD Info",
        WS_OVERLAPPEDWINDOW, // Include resizing functionality
        //CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, // Adaptive size
        CW_USEDEFAULT, CW_USEDEFAULT, 1000, 800,
        NULL, NULL, hInstance, NULL
    );

    if (hwnd == NULL) {
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);

    MSG msg = { };
    // retrieve messages from the application's message queue
    while (GetMessage(&msg, NULL, 0, 0)) {
        // Convert key stokes into character message
        TranslateMessage(&msg);
        // Send the message to the appropriate window procedure
        DispatchMessage(&msg);
    }

    return 0;
}
