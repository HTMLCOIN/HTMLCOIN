#include <qtum/qtumDGP.h>
#include <chainparams.h>

std::vector<uint32_t> createDataSchedule(const dev::eth::EVMSchedule& schedule)
{
    std::vector<uint32_t> tempData = {schedule.tierStepGas[0], schedule.tierStepGas[1], schedule.tierStepGas[2],
                                      schedule.tierStepGas[3], schedule.tierStepGas[4], schedule.tierStepGas[5],
                                      schedule.tierStepGas[6], schedule.tierStepGas[7], schedule.expGas,
                                      schedule.expByteGas, schedule.sha3Gas, schedule.sha3WordGas,
                                      schedule.sloadGas, schedule.sstoreSetGas, schedule.sstoreResetGas,
                                      schedule.sstoreRefundGas, schedule.jumpdestGas, schedule.logGas,
                                      schedule.logDataGas, schedule.logTopicGas, schedule.createGas,
                                      schedule.callGas, schedule.callStipend, schedule.callValueTransferGas,
                                      schedule.callNewAccountGas, schedule.suicideRefundGas, schedule.memoryGas,
                                      schedule.quadCoeffDiv, schedule.createDataGas, schedule.txGas,
                                      schedule.txCreateGas, schedule.txDataZeroGas, schedule.txDataNonZeroGas,
                                      schedule.copyGas, schedule.extcodesizeGas, schedule.extcodecopyGas,
                                      schedule.balanceGas, schedule.suicideGas, schedule.maxCodeSize};
    return tempData;
}

std::vector<uint32_t> scheduleDataForBlockNumber(unsigned int blockHeight)
{
    dev::eth::EVMSchedule schedule = globalSealEngine->chainParams().scheduleForBlockNumber(blockHeight);
    return createDataSchedule(schedule);
}

void QtumDGP::initDataSchedule(){
    dataSchedule = scheduleDataForBlockNumber(0);
}

bool QtumDGP::checkLimitSchedule(const std::vector<uint32_t>& defaultData, const std::vector<uint32_t>& checkData, int blockHeight){
    const Consensus::Params& consensusParams = Params().GetConsensus();

    if(defaultData.size() == 39 && (checkData.size() == 39 || (checkData.size() == 40 && blockHeight >= consensusParams.QIP7Height))) {
        for(size_t i = 0; i < defaultData.size(); i++){
            uint32_t max = defaultData[i] * 1000 > 0 ? defaultData[i] * 1000 : 1 * 1000;
            uint32_t min = defaultData[i] / 100 > 0 ? defaultData[i] / 100 : 1;
            if(checkData[i] > max || checkData[i] < min){
                return false;
            }
        }
        return true;
    }
    return false;
}

dev::eth::EVMSchedule QtumDGP::getGasSchedule(int blockHeight){
    clear();
    dataSchedule = scheduleDataForBlockNumber(blockHeight);

    return globalSealEngine->chainParams().scheduleForBlockNumber(blockHeight);
}

uint32_t QtumDGP::getBlockSize(unsigned int blockHeight){
    clear();
    return DEFAULT_BLOCK_SIZE_DGP;
}

uint64_t QtumDGP::getMinGasPrice(unsigned int blockHeight){
    clear();
    return DEFAULT_MIN_GAS_PRICE_DGP;
}

uint64_t QtumDGP::getBlockGasLimit(unsigned int blockHeight){
    clear();
    return DEFAULT_BLOCK_GAS_LIMIT_DGP;
}

void QtumDGP::createParamsInstance(){
    dev::h256 paramsInstanceHash = sha3(dev::h256("0000000000000000000000000000000000000000000000000000000000000000"));
    if(storageDGP.count(paramsInstanceHash)){
        dev::u256 paramsInstanceSize = storageDGP.find(paramsInstanceHash)->second.second;
        for(size_t i = 0; i < size_t(paramsInstanceSize); i++){
            std::pair<unsigned int, dev::Address> params;
            params.first = dev::toUint64(storageDGP.find(sha3(paramsInstanceHash))->second.second);
            ++paramsInstanceHash;
            params.second = dev::right160(dev::h256(storageDGP.find(sha3(paramsInstanceHash))->second.second));
            ++paramsInstanceHash;
            paramsInstance.push_back(params);
        }
    }
}

dev::Address QtumDGP::getAddressForBlock(unsigned int blockHeight){
    for(auto i = paramsInstance.rbegin(); i != paramsInstance.rend(); i++){
        if(i->first <= blockHeight)
            return i->second;
    }
    return dev::Address();
}

static inline bool sortPairs(const std::pair<dev::u256, dev::u256>& a, const std::pair<dev::u256, dev::u256>& b){
    return a.first < b.first;
}

void QtumDGP::parseStorageScheduleContract(std::vector<uint32_t>& uint32Values){
    std::vector<std::pair<dev::u256, dev::u256>> data;
    for(size_t i = 0; i < 5; i++){
        dev::h256 gasScheduleHash = sha3(dev::h256(dev::u256(i)));
        if(storageTemplate.count(gasScheduleHash)){
            dev::u256 key = storageTemplate.find(gasScheduleHash)->second.first;
            dev::u256 value = storageTemplate.find(gasScheduleHash)->second.second;
            data.push_back(std::make_pair(key, value));
        }
    }

    std::sort(data.begin(), data.end(), sortPairs);

    for(std::pair<dev::u256, dev::u256> d : data){
        dev::u256 value = d.second;
        for(size_t i = 0; i < 4; i++){
            uint64_t uint64Value = dev::toUint64(value);
            value = value >> 64;

            uint32Values.push_back(uint32_t(uint64Value));
            uint64Value = uint64Value >> 32;
            uint32Values.push_back(uint32_t(uint64Value));
        }
    }
}

void QtumDGP::parseDataScheduleContract(std::vector<uint32_t>& uint32Values){
    size_t size = dataTemplate.size() / 32;
    for(size_t i = 0; i < size; i++){
        std::vector<unsigned char> value = std::vector<unsigned char>(dataTemplate.begin() + (i * 32), dataTemplate.begin() + ((i+1) * 32));
        dev::h256 valueTemp(value);
        uint32Values.push_back(dev::toUint64(dev::u256(valueTemp)));
    }
}

dev::eth::EVMSchedule QtumDGP::createEVMSchedule(const dev::eth::EVMSchedule &_schedule, int blockHeight){
    dev::eth::EVMSchedule schedule = _schedule;
    std::vector<uint32_t> uint32Values;

    if(!dgpevm){
        parseStorageScheduleContract(uint32Values);
    } else {
        parseDataScheduleContract(uint32Values);
    }

    if(!checkLimitSchedule(dataSchedule, uint32Values, blockHeight))
        return schedule;

    if(uint32Values.size() >= 39){
        schedule.tierStepGas = {{uint32Values[0], uint32Values[1], uint32Values[2], uint32Values[3],
                                uint32Values[4], uint32Values[5], uint32Values[6], uint32Values[7]}};
        schedule.expGas = uint32Values[8];
        schedule.expByteGas = uint32Values[9];
        schedule.sha3Gas = uint32Values[10];
        schedule.sha3WordGas = uint32Values[11];
        schedule.sloadGas = uint32Values[12];
        schedule.sstoreSetGas = uint32Values[13];
        schedule.sstoreResetGas = uint32Values[14];
        schedule.sstoreRefundGas = uint32Values[15];
        schedule.jumpdestGas = uint32Values[16];
        schedule.logGas = uint32Values[17];
        schedule.logDataGas = uint32Values[18];
        schedule.logTopicGas = uint32Values[19];
        schedule.createGas = uint32Values[20];
        schedule.callGas = uint32Values[21];
        schedule.callStipend = uint32Values[22];
        schedule.callValueTransferGas = uint32Values[23];
        schedule.callNewAccountGas = uint32Values[24];
        schedule.suicideRefundGas = uint32Values[25];
        schedule.memoryGas = uint32Values[26];
        schedule.quadCoeffDiv = uint32Values[27];
        schedule.createDataGas = uint32Values[28];
        schedule.txGas = uint32Values[29];
        schedule.txCreateGas = uint32Values[30];
        schedule.txDataZeroGas = uint32Values[31];
        schedule.txDataNonZeroGas = uint32Values[32];
        schedule.copyGas = uint32Values[33];
        schedule.extcodesizeGas = uint32Values[34];
        schedule.extcodecopyGas = uint32Values[35];
        schedule.balanceGas = uint32Values[36];
        schedule.suicideGas = uint32Values[37];
        schedule.maxCodeSize = uint32Values[38];
    }
    return schedule;
}

void QtumDGP::clear(){
    templateContract = dev::Address();
    storageDGP.clear();
    storageTemplate.clear();
    paramsInstance.clear();
}
