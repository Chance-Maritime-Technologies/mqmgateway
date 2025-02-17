#include "config.hpp"
#include "common.hpp"
#include "yaml_converters.hpp"

namespace modmqttd {

#if __cplusplus < 201703L
    constexpr std::chrono::milliseconds ModbusNetworkConfig::MAX_RESPONSE_TIMEOUT;
#endif

ConfigurationException::ConfigurationException(const YAML::Mark& mark, const char* what) {
    mWhat = "config error";
    if (mark.is_null()) {
        mWhat += ": ";
    } else {
        mWhat += "(line ";
        mWhat += std::to_string(mark.line);
        mWhat += "): ";
    }
    mWhat += what;
}

ModbusNetworkConfig::ModbusNetworkConfig(const YAML::Node& source) {
    mName = ConfigTools::readRequiredString(source, "name");

    YAML::Node rtNode(ConfigTools::setOptionalValueFromNode<std::chrono::milliseconds>(this->mResponseTimeout, source, "response_timeout"));
    if (rtNode.IsDefined()) {
        if ((this->mResponseTimeout < std::chrono::milliseconds::zero()) || (this->mResponseTimeout > MAX_RESPONSE_TIMEOUT))
            throw ConfigurationException(rtNode.Mark(), "response_timeout value must be in range 0-999ms");
    }

    YAML::Node rtdNode(ConfigTools::setOptionalValueFromNode<std::chrono::milliseconds>(this->mResponseDataTimeout, source, "response_data_timeout"));
    if (rtdNode.IsDefined()) {
        if ((this->mResponseDataTimeout < std::chrono::milliseconds::zero()) || (this->mResponseDataTimeout > MAX_RESPONSE_TIMEOUT))
            throw ConfigurationException(rtdNode.Mark(), "response_data_timeout value must be in range 0-999ms");
    }

    ConfigTools::readOptionalValue<std::chrono::milliseconds>(this->mMinDelayBeforePoll, source, "min_delay_before_poll");

    if (source["device"]) {
        mType = Type::RTU;
        mDevice = ConfigTools::readRequiredString(source, "device");
        mBaud = ConfigTools::readRequiredValue<int>(source, "baud");
        mParity = ConfigTools::readRequiredValue<char>(source, "parity");
        mDataBit = ConfigTools::readRequiredValue<int>(source, "data_bit");
        mStopBit = ConfigTools::readRequiredValue<int>(source, "stop_bit");
        ConfigTools::readOptionalValue<RtuSerialMode>(this->mRtuSerialMode, source, "rtu_serial_mode");
        ConfigTools::readOptionalValue<RtuRtsMode>(this->mRtsMode, source, "rtu_rts_mode");
        ConfigTools::readOptionalValue<int>(this->mRtsDelayUs, source, "rtu_rts_delay_us");
    } else if (source["address"]) {
        mType = Type::TCPIP;
        mAddress = ConfigTools::readRequiredString(source, "address");
        mPort = ConfigTools::readRequiredValue<int>(source, "port");
    } else {
        throw ConfigurationException(source.Mark(), "Cannot determine modbus network type: missing 'device' or 'address'");
    }
}

MqttBrokerConfig::MqttBrokerConfig(const YAML::Node& source) {
    mHost = ConfigTools::readRequiredString(source, "host");
    ConfigTools::readOptionalValue<int>(mPort, source, "port");
    ConfigTools::readOptionalValue<int>(mKeepalive, source, "keepalive");
    ConfigTools::readOptionalValue<std::string>(mUsername, source, "username");
    ConfigTools::readOptionalValue<std::string>(mPassword, source, "password");
}


}
