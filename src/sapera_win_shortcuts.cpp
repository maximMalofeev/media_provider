#include "sapera_win_shortcuts.h"

const std::string DALSA_VENDOR_NAME = "Teledyne DALSA";
const std::string PIXEL_FORMAT_FEATURE_NAME = "PixelFormat";
const std::string WIDTH_FEATURE_NAME = "Width";
const std::string HEIGHT_FEATURE_NAME = "Height";
const std::string ACQUISITION_FRAME_RATE_FEATURE_NAME = "AcquisitionFrameRate";
const std::string PACKET_SIZE_FEATURE_NAME = "GevSCPSPacketSize";
const std::string VENDOR_FEATURE_NAME = "DeviceVendorName";
const std::string MODEL_FEATURE_NAME = "DeviceModelName";
const std::string DEVICE_VERSION_FEATURE_NAME = "DeviceVersion";
const std::string FIRMWARE_VERSION_FEATURE_NAME = "DeviceFirmwareVersion";
const std::string SERIAL_NUMBER_FEATURE_NAME = "DeviceSerialNumber";
const std::string GEV_CURRENT_IP_ADDRESS_FEATURE_NAME = "GevCurrentIPAddress";
const std::string EXPOSURE_TIME_FEATURE_NAME = "ExposureTime";
const std::string GAIN_FEATURE_NAME = "Gain";
const std::string GAIN_RAW_FEATURE_NAME = "GainRaw";
const std::string GAIN_SELECTOR_FEATURE_NAME = "GainSelector";
const std::string CYCLING_PRESET_MODE_NAME = "cyclingPresetMode";
const std::string CP_EXPOSURE_TIME_FEATURE_NAME = "cP_ExposureTime";
const std::string CP_PRESET_CONFIGURATION_SELECTOR_FEATURE_NAME = "cP_PresetConfigurationSelector";

template<> bool getFeatureValue(SapAcqDevice* AcqDevice, const std::string& featureName, std::string& featureValue, std::string& error) {
	BOOL isFeatureAvailable = 0;
	error = "";
	char fVal[40];
	if (AcqDevice->IsFeatureAvailable(featureName.c_str(), &isFeatureAvailable), isFeatureAvailable) {
		if (AcqDevice->GetFeatureValue(featureName.c_str(), fVal, sizeof(fVal))) {
			featureValue = fVal;
			return true;
		}
		else {
			error.append("Unable to get").append(featureName.c_str()).append(", reason: ").append(AcqDevice->GetLastStatus());
			return false;
		}
	}
	else {
		error.append("Feature ").append(featureName.c_str()).append(" isnt available").append(", reason: ").append(AcqDevice->GetLastStatus());
		return false;
	}
}

template<> bool setFeatureValue(SapAcqDevice* AcqDevice, const std::string& featureName, const std::string& featureValue, std::string& error) {
	BOOL isAvailable = 0;
	error = "";
	if (AcqDevice->IsFeatureAvailable(featureName.c_str(), &isAvailable), isAvailable) {
		if (AcqDevice->SetFeatureValue(featureName.c_str(), featureValue.c_str())) {
			return true;
		}
		else {
			error.append("Unable to set").append(featureName.c_str()).append(", reason: ").append(AcqDevice->GetLastStatus());
			return false;
		}
	}
	else {
		error.append("Feature ").append(featureName.c_str()).append(" isnt available").append(", reason: ").append(AcqDevice->GetLastStatus());
		return false;
	}
}
