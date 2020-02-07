#pragma once
#include <SapClassBasic.h>
#include <map>
#include <string>

extern const std::string DALSA_VENDOR_NAME;
extern const std::string PIXEL_FORMAT_FEATURE_NAME;
extern const std::string WIDTH_FEATURE_NAME;
extern const std::string HEIGHT_FEATURE_NAME;
extern const std::string ACQUISITION_FRAME_RATE_FEATURE_NAME;
extern const std::string PACKET_SIZE_FEATURE_NAME;
extern const std::string VENDOR_FEATURE_NAME;
extern const std::string MODEL_FEATURE_NAME;
extern const std::string DEVICE_VERSION_FEATURE_NAME;
extern const std::string FIRMWARE_VERSION_FEATURE_NAME;
extern const std::string SERIAL_NUMBER_FEATURE_NAME;
extern const std::string GEV_CURRENT_IP_ADDRESS_FEATURE_NAME;
extern const std::string EXPOSURE_TIME_FEATURE_NAME;
extern const std::string GAIN_FEATURE_NAME;
extern const std::string GAIN_RAW_FEATURE_NAME;
extern const std::string GAIN_SELECTOR_FEATURE_NAME;
extern const std::string CYCLING_PRESET_MODE_NAME;
extern const std::string CP_EXPOSURE_TIME_FEATURE_NAME;
extern const std::string CP_PRESET_CONFIGURATION_SELECTOR_FEATURE_NAME;

template <typename T>
bool getFeatureValue(SapAcqDevice* AcqDevice, const std::string& featureName,
                     T& value, std::string& error) {
  BOOL isFeatureAvailable = 0;
  error = "";
  if (AcqDevice->IsFeatureAvailable(featureName.c_str(), &isFeatureAvailable),
      isFeatureAvailable) {
    if (AcqDevice->GetFeatureValue(featureName.c_str(), &value)) {
      return true;
    } else {
      error.append("Unable to get")
          .append(featureName.c_str())
          .append(", reason: ")
          .append(AcqDevice->GetLastStatus());
      return false;
    }
  } else {
    error.append("Feature ")
        .append(featureName.c_str())
        .append(" isnt available")
        .append(", reason: ")
        .append(AcqDevice->GetLastStatus());
    return false;
  }
}

template <typename T>
bool setFeatureValue(SapAcqDevice* AcqDevice, const std::string& featureName,
                     const T& featureValue, std::string& error) {
  BOOL isAvailable = 0;
  error = "";
  if (AcqDevice->IsFeatureAvailable(featureName.c_str(), &isAvailable),
      isAvailable) {
    if (AcqDevice->SetFeatureValue(featureName.c_str(), featureValue)) {
      return true;
    } else {
      error.append("Unable to set ")
          .append(featureName.c_str())
          .append(", reason: ")
          .append(AcqDevice->GetLastStatus());
      return false;
    }
  } else {
    error.append("Feature ")
        .append(featureName.c_str())
        .append(" isnt available")
        .append(", reason: ")
        .append(AcqDevice->GetLastStatus());
    return false;
  }
}

template <>
bool getFeatureValue(SapAcqDevice* AcqDevice, const std::string& featureName,
                     std::string& featureValue, std::string& error);
template <>
bool setFeatureValue(SapAcqDevice* AcqDevice, const std::string& featureName,
                     const std::string& featureValue, std::string& error);

template <typename T>
bool getMinAndMaxValues(SapAcqDevice* AcqDevice, const std::string serverName,
                        const std::string& featureName, T& min, T& max,
                        std::string& error) {
  BOOL isAvailable = 0;
  error = "";

  SapFeature feature(SapManager::GetServerIndex(serverName.c_str()));
  if (!feature.Create()) {
    error.append("Unable to create feature object for ")
        .append(featureName)
        .append(", reason: ")
        .append(feature.GetLastStatus());
    return false;
  }

  if (AcqDevice->IsFeatureAvailable(featureName.c_str(), &isAvailable),
      isAvailable) {
    if (AcqDevice->GetFeatureInfo(featureName.c_str(), &feature)) {
      if (!feature.GetMin(&min)) {
        error.append("Unable to get min ")
            .append(featureName)
            .append(", reason: ")
            .append(feature.GetLastStatus());
        return false;
      }
      if (!feature.GetMax(&max)) {
        error.append("Unable to get max ")
            .append(featureName)
            .append(", reason: ")
            .append(feature.GetLastStatus());
        return false;
      }
      return true;
    } else {
      error.append("Unable to get ")
          .append(featureName)
          .append(" info, reason: ")
          .append(feature.GetLastStatus());
      return false;
    }
  } else {
    error.append("Feature ")
        .append(featureName.c_str())
        .append(" isnt available")
        .append(", reason: ")
        .append(AcqDevice->GetLastStatus());
    return false;
  }
}

template <typename T>
bool setFeatureValueInAvaliableRange(SapAcqDevice* AcqDevice,
                                     const std::string serverName,
                                     const std::string& featureName,
                                     T& featureValue, std::string& error) {
  T minValue = T();
  T maxValue = T();
  if (!getMinAndMaxValues<T>(AcqDevice, serverName, featureName, minValue,
                             maxValue, error)) {
    return false;
  }

  if (featureValue < minValue || featureValue > maxValue) {
    featureValue < minValue ? featureValue = minValue
                            : featureValue = maxValue - 1;
  }

  if (!setFeatureValue<T>(AcqDevice, featureName, featureValue, error)) {
    return false;
  }

  return true;
}
