#include "Axis.h"

#include <iostream>
#include <chrono>

void Axis::findIndex()
{
    sendCommand_("INDX", 0);
    was_valid_DPOS_ = false;
    while (!isEncoderValid()) {
        waitForUpdate_();
        if (!isSearchingIndex()) {
            std::cout << "Index is not found, but stopped searching for index." << std::endl;
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }

    if (isEncoderValid()) {
        std::cout << "Index of axis " << letter_ << " found." << std::endl;
    }
}

bool Axis::isWithinTol_(int DPOS) {
    DPOS = std::abs(DPOS);
//    int PTO2 = getData("PTO2");
    int PTO2 = getSetting("PTO2");
    int EPOS = std::abs(getData("EPOS"));
    std::cout << "x EPOS = " << EPOS << ":" << DPOS << ":" << PTO2 << std::endl;
    if (((DPOS - PTO2) <= EPOS) && (EPOS <= (DPOS + PTO2))) {
        std::cout << "true isWithinTol_" << std::endl;
        return true;
    }
    return false;
}

std::time_t Axis::getActualTime_()
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

bool Axis::timeOutReached_(std::time_t start_time, int distance)
{
    auto now_ms = getActualTime_();
	int speed = getSetting("SSPD");
    double timeout = 0.0;
//    if(speed != 0) {
        timeout = (distance / speed * 1000);
//    }
    timeout *= 1.25;
    // For quick and tiny movements, the method above is not accurate.
    // If the timeout is smaller than the specified TOUT&TOU2, use TOUT+TOU2
    if ((settings_.count("TOU2") != 0) && (settings_.count("TOUT") != 0)) {
        int TOUT = getSetting("TOU2") + getSetting("TOUT");
        if (TOUT > timeout)
            timeout = TOUT;
    }
    return (now_ms - start_time) > timeout;
}

void Axis::setDPOS(Distance d)
{

    int DPOS = d / stage_->getEncoderResolution();

    std::cout << "d = " << DPOS <<"  " <<stage_->getEncoderResolution()<<std::endl;
    
    sendCommand_("DPOS", DPOS);

    was_valid_DPOS_ = true;

    int distance = std::abs(DPOS - getData("EPOS"));
    auto send_time = getActualTime_();
    bool error = false;

    waitForUpdate_();
//    int k = 0;
    while (!(isWithinTol_(DPOS) && isPositionReached())) {
        // Check if stage is at left end or right end. ==> out of range movement.
        if (isAtLeftEnd() || isAtRightEnd()) {
            std::cout << "DPOS is out of range. (1) " << d << std::endl;
            error = true;
            break;
        }
        // Position reached flag is set, but EPOS not within tolerance of DPOS.
        if (!isWithinTol_(DPOS) && isPositionReached()) {
            // Check if it's a lineair stage and DPOS is beyond it's limits.
            if (stage_->isLinear() && ((getSetting("LLIM") > DPOS) || (getSetting("HLIM") < DPOS))) {
                std::cout << "DPOS is out of range. (2) " << d << std::endl;
                error = true;
                break; 
            }

            // EPOS is not within tolerance of DPOS, unknown reason.
            std::cout << "Position not reached. (3) " << d << std::endl;
            error = true;
            break;
        }

        if (timeOutReached_(send_time, distance)) {
            std::cout << "Position not reached, timeout reached. (4) " << d << std::endl;
            error = true;
            break;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
//        k++;
//        std::cout << "k = " << k << std::endl;
//        if(k == 41) {
//            break;
//        }
    }
    
    // Removed these lines, in order to avoid an accumulating error when stepping. 
    //if (error)
    //    was_valid_DPOS_ = false;
}

void Axis::step(Distance d) {
    Distance new_DPOS = 0_nm;

    if (was_valid_DPOS_)
        new_DPOS = getDPOS() + d;
    else
        new_DPOS = getEPOS() + d;
       
    if (!stage_->isLinear()) {
        if (new_DPOS < 0)
            new_DPOS = /* encCountsPerRev_ */ 0 - std::fabs(new_DPOS);
        if (new_DPOS > /* encCountsPerRev_ */ 0)
            new_DPOS -= /* encCountsPerRev_ */ 0;
    }

    setDPOS(new_DPOS);
    waitForUpdate_();
    std::cout << "Stepped " << d << " " << getData("DPOS") << std::endl;
}

void Axis::sendCommand_(const char * command)
{
    xeryon_->sendCommand(this, command);
}

void Axis::sendCommand_(std::string tag, int value)
{
    char cmd[64];
    snprintf(cmd, sizeof(cmd), "%s=%d", tag.c_str(), value);

    xeryon_->sendCommand(this, cmd);
}

void Axis::sendCommand(std::string tag, int value)
{
    if (not_settings_command_.count(tag) > 0)
        sendCommand_(tag, value);
    else
        setSetting(tag, value);
}

void Axis::sendSettings()
{
    char cmd[64];
    for (auto const& item : settings_)
    {
        sendCommand_(item.first.c_str(), item.second);
        std::this_thread::sleep_for(std::chrono::milliseconds(20));

    }
    sendCommand_(stage_->getEncoderResolutionCommand());
}

int Axis::applySettingsMultipliers_(std::string tag, int value)
{
    if (tag == "MAMP" || tag == "OSFA" || tag == "OFSB" || tag == "AMPL" || tag == "MAM2"|| tag == "MAM3")
        // Use amplitude multiplier.
        value *= stage_->getAmplitudeMultiplier();
    else if (tag == "PHAC" || tag == "PHAS")
        value *= stage_->getPhaseMultiplier();
    else if (tag ==	"SSPD" || tag == "MSPD")
        // In the settigns file, SSPD is in mm/s ==> gets translated to mu/s
        value *= stage_->getSpeedMultiplier();
    else if (tag == "LLIM" || tag == "RLIM" || tag == "HLIM" || tag == "ZON1" || tag == "ZON2" || tag == "ZON3")
        // These are given in mm and need to be converted to encoder units
       {
        value = Distance(value, Distance::MM) / stage_->getEncoderResolution();
//        std::cout << "encoder: " << stage_->getEncoderResolution() << std::endl;
    }
    else if (tag == "POLI")
        def_poli_value_ = value;
    else if (tag == "MASS") {
        if (value <= 50)
            value = 100000;
        else if (value <= 100)
            value = 60000;
        else if (value <= 250)
            value = 30000;
        else if (value <= 500)
            value = 10000;
        else if (value <= 1000)
            value = 5000;
        else
			value = 3000;
	}
    return value;
}

void Axis::setSetting(std::string tag, std::string value, bool fromSettingsFile)
{
    setSetting(tag, std::stoi(value), fromSettingsFile);
}

void Axis::setSetting(std::string tag, int value, bool fromSettingsFile)
{
    if (fromSettingsFile) {
        value = applySettingsMultipliers_(tag, value);
        if (tag == "MASS")
            tag = "CFRQ";
    }
    settings_[tag] = value;
    if (!fromSettingsFile)
        sendCommand_(tag.c_str(), value);
}

void Axis::stopMovement()
{
    sendCommand_("STOP", 1);
    was_valid_DPOS_ = false;
	
}

void Axis::stop()
{
    sendCommand_("ZERO", 0);
    sendCommand_("RSET", 0);
    sendCommand_("STOP", 1);
    was_valid_DPOS_ = false;
}

void Axis::reset()
{
    sendCommand_("RSET", 0);
    was_valid_DPOS_ = false;
}

void Axis::receiveData(std::string tag, int value)
{
    if (tag == "TIME")
        receiveData("PCTIME", getActualTime_());

    data_[tag] = value;

//    std::cout << "Received: " << letter_ << " " << tag << " = " << value << std::endl;

    // This uses "EPOS" as an indicator that a new round of data is coming in.
    if (tag == "EPOS")
        update_nb_++;    // This update_nb is for the function waitForUpdate_

    if (is_logging_) {
        if (tag != "SRNO" && tag != "XLS " && tag != "XRTU" && tag != "XLA " &&
            tag != "XTRA" && tag != "SOFT" && tag != "SYNC") {
#if 0
            // TODO: logs_[tag].append(val)
            // std::map<std::string, std::vector<int>> logs_;
#endif
        }
    }
}

void Axis::waitForUpdate_()
{
    int wait_nb = 3;
	if (settings_.count("POLI"))
        wait_nb = wait_nb * getSetting("POLI") / def_poli_value_;

    int start_nb = update_nb_;
    while (update_nb_ - start_nb < wait_nb)
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
}

void Axis::startScan(int direction, int execTime)
{
    sendCommand_("SCAN", direction);
    was_valid_DPOS_ = false;

    if (execTime) {
        std::this_thread::sleep_for(std::chrono::seconds(execTime));
        sendCommand_("SCAN", 0);
    }
}


void Axis::stopScan()
{
    sendCommand_("SCAN", 0);
    was_valid_DPOS_ = false;
}

void Axis::calibrate()
{
    sendCommand_("FFRQ", 0);
    was_valid_DPOS_ = false;

    waitForUpdate_();
    int LFRQ = getSetting("LFRQ");
    int HFRQ = getSetting("HFRQ");
    std::cout << "Start calibrating..." << std::endl;
    while (isSearchingOptimalFrequency()) {
        int FRQ = getData("FREQ");
        int freq_ratio = ((FRQ - LFRQ) * 100) / (HFRQ - LFRQ); // Calculate % finished
        if (freq_ratio % 10 == 0) {
            std::cout << "Calibrating: " << freq_ratio << "%" << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
    std::cout << "Calibrating finished." << std::endl;
}

void Axis::setSpeed(Distance speed)
{
    int s = speed(Distance::MU);

#if 0 // TODO
    if (!stage_->isLinear())
        s = speed(Distance::DEG) * 100;
#endif

    sendCommand("SSPD", s);
}

void Axis::startLogging()
{
    is_logging_ = true;
    setSetting("POLI", 1);
    waitForUpdate_();
}

void Axis::endLogging()
{
    is_logging_ = false;
    // TODO: handle log
    setSetting("POLI", DEFAULT_POLI_VALUE);
    // TODO: return log; 
}

