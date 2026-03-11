#include "rwDb.h"
#include "HGExactTime.h"
#include <iomanip>
#include <memory>
#include <sstream>
#include "hgcommonutility.h"
#include "config.h"
#include "hglog4cplus.h"

namespace HGMACHINE{
    std::vector<std::string> userInfoName={
    "UserNo",
    "UserAccount",
    "Password",
    "PasswdInputLimitCount",
    "PasswdCycle",
    "UserName",
    "UserJob",
    "Department",
    "Authority",
    "AccountManagement",
    "UserDescription",
    "Creator",
    "CreateTime",
    "LastModifier",
    "LastModifyTime",
    "LoginTime",
    "QuitTime"
};
std::vector<std::string> userGroupInfoName={
        "GroupName",
        "GroupPNumber",
        "GroupPerson",
        "Authority",
        "Creator",
        "CreateTime",
        "LastModifier",
        "LastModifyTime"
    };
    std::vector<std::string> scannerInfoName={
        "Sname","Sbatch","Sserial","Samount","Separator"
    };
    std::vector<std::string> methodInfoName={
        "序号","名称","类型","创建时间","DBName","参数"
    };
    std::vector<std::string> flowInfoName={
        "序号","名称","类型","创建时间","DBName"
    };
    std::vector<std::string> taskInfoName={"序号","样品来源","取样方式","样品名称","目标成分","样品总量","单位",\
      "condition1","value1","condition2","value2","通道","流程","方法","工作模式",\
        "间隔时间","取样管路","状态"
    };
    std::vector<std::string> channelInfoName={
        "序号","通道","检测器","检测方法","DBName"
    };
std::vector<std::string> reagentInfoName={
    "序号","试剂类别","试剂编码","试剂名称","滴定度浓度","标定日期","标定周期","试剂余量","当前状态","DBName"
};

const std::vector<std::string> reagentNoEditName={
    "序号","标定日期","试剂余量","当前状态"
};
const std::vector<std::string> reagentLinkDeviceNoEditName={
    "序号","联机","当前通道"
};
const std::vector<std::string> reagentLinkDeviceName={
    "序号","指标设备单元","接口","联机","当前通道","选择"
};
    std::map<int,std::string> taskWorkingMode={
        {0,"周期性测定"},
        {1,"循环测试"},
        {2,"定时测试"}
    };
    std::map<int,std::string> taskInterval={
        {0,"30min"}
    };
    RWDb::RWDb(){

    }
    RWDb::~RWDb(){

    }

    HGSaveDataToDB RWDb::dbOpera;
    HGSaveDataToDB RWDb::dataOpera;
    HGSaveDataToDB RWDb::logOpera;

    void RWDb::openDB(const std::string &basePath){
        dbOpera.openDB(basePath+DB_PATH);
        dataOpera.openDB(basePath+DATA_DB_PATH);
        logOpera.openDB(basePath+LOG_DB_PATH);
    }
    void RWDb::closeDB(){
        dbOpera.closeDB();
        dataOpera.closeDB();
        logOpera.closeDB();
    }
    void RWDb::clearDB(const std::string& tableName){
        dbOpera.clearDB(tableName);
    }
    std::string RWDb::readCurDirPath(){
        std::string path=FileConfig::getDirPath(); 
        return path;
    }
    std::string RWDb::readLoginName()
    {
        std::string curDirPath = RWDb::readCurDirPath();
        std::map<std::string, std::string> noLoginRecordInfo;
        noLoginRecordInfo["loginName"] = "";
        dbOpera.readSingleInfo(SYSTEMINFODBNAME, noLoginRecordInfo);
        return noLoginRecordInfo["loginName"];
    }
    int RWDb::readAuditTrailLogCount(const std::string &tableName){
        std::map<std::string,std::string> info;
        info["lastAuditTrailDB"]="";
        logOpera.readSingleInfo(AUDITTRAILDBRECORD,info);
        std::map<std::string,std::string> infoS = {
            {"Operator",""},
            {"Time",""},
            {"LogContent",""}
        };
        std::string readTableName="";
        if (tableName != ""){
            readTableName = tableName;
        } else {
            readTableName = info["lastAuditTrailDB"];
        }
        return logOpera.countOfTable(readTableName);
    }
    void RWDb::recordSingleInfo(std::string dbName, const std::map<std::string, std::string> &infoS)
    {
        dbOpera.recordSingleInfo(dbName, infoS);
    }

    std::vector<std::map<std::string, std::string>> RWDb::readTaskInfo(const std::string& taskSeqName)
    {
        std::vector<std::map<std::string, std::string>> fillContent;
        std::map<std::string, std::string> infoS;
        for (int i = 0; i < int(taskInfoName.size()); i++)
        {
            infoS[taskInfoName[i]] = "";
        }
        fillContent = readRecord(taskSeqName/*HgOnlinePlatformModule::getTaskSeqName()*/, infoS);
        return fillContent;
    }

    void RWDb::writeTaskRecord(bool coverFlag, const std::string& tableName,const std::vector<std::map<std::string, std::string>> &infoSS)
    {
        std::map<std::string, std::string> infoS;
        if (coverFlag)
        {
            // dbOpera.deleteDB(readCurDirPath() + DB_PATH, HgOnlinePlatformModule::getTaskSeqName());
        }
        for (const auto &infoS : infoSS)
        {
            dbOpera.writeRecord(tableName, "序号", infoS);
        }
        dbOpera.sortTable(tableName,"序号");
    }
    std::vector<std::map<std::string, std::string>> RWDb::readFlowInfo()
    {
        std::vector<std::map<std::string, std::string>> fillContent;
        std::map<std::string, std::string> infoS;
        for (int i = 0; i < int(flowInfoName.size()); i++)
        {
            infoS[flowInfoName[i]] = "";
        }
        fillContent = readRecord(FLOWMANAGEDBNAME, infoS);

        return fillContent;
    }
    std::vector<std::map<std::string, std::string>> RWDb::readMethodInfo()
    {
        std::vector<std::map<std::string, std::string>> fillContent;
        std::map<std::string, std::string> infoS;
        for (int i = 0; i < int(methodInfoName.size()); i++)
        {
            infoS[methodInfoName[i]] = "";
        }
        fillContent =readRecord(METHODMANAGENAME, infoS);
        return fillContent;
    }

    Method RWDb::readMethodParam(std::string dbName){
        Method method;
        std::vector<std::map<std::string,std::string>> fillContent;
        std::map<std::string,std::string> info;
        info["序号"]="";
        info["参数"]="";
        fillContent=readRecord(dbName,info);
        if (fillContent.size()>0){
            method.param=fillContent[0]["参数"];
        }
        return method;
    }
    std::vector<std::string> RWDb::getMethodNames(){
        std::vector<std::map<std::string,std::string>> fillContent=readMethodInfo();
        std::vector<std::string> names;
        for (int i=0;i<int(fillContent.size());i++){    
            names.push_back(fillContent[i]["序号"]+"_"+fillContent[i]["名称"]);
        }
        return names;
    }

std::string RWDb::getMethodName(const std::string &flowName){
    std::string methodName="";
    // first find flow
    std::vector<std::map<std::string,std::string>> fillContent=readFlowInfo();
    std::vector<std::string> names;
    std::string dbName="";
    for (int i=0;i<int(fillContent.size());i++){
        std::string tempName=fillContent[i]["序号"]+"_"+fillContent[i]["名称"];
        if (tempName==flowName){
            dbName=fillContent[i]["DBName"];
            break;
        }
    }
    if (dbName=="") return methodName;
    FlowOfTask flowOfTask=RWDb::readFlowOfTask(dbName);
    for (int i=0;i<int(flowOfTask.steps.size());i++){
        if (flowOfTask.steps[i].name=="测定"){
            std::map<std::string, std::string> wparam = getParamMap(flowOfTask.steps[i].param);
            methodName+="["+wparam["测试方法"]+"]";
        }
    }
    return methodName; 
}

    void RWDb::writeFlowRecord(std::string dbName, bool coverFlag, const std::vector<std::map<std::string, std::string>> &infoSS)
    {
        std::map<std::string, std::string> infoS;
        // int count=HGSaveDataToDB::countOfTable(HgOnlinePlatformModule::getTaskSeqName());
        if (coverFlag)
        {
            dbOpera.clearDB(dbName);
        }
        if (infoSS.size() <= 0)
        {
            infoS["序号"] = "";
            infoS["名称"] = "";
            infoS["参数"] = "";
            dbOpera.createDB(dbName, "序号", infoS);
        }
        for (const auto &infoS : infoSS)
        {
            dbOpera.writeRecord(dbName, "序号", infoS);
        }
        dbOpera.sortTable(dbName,"序号");
    }
    void RWDb::writeMethodRecord(std::string dbName, bool coverFlag, const std::vector<std::map<std::string, std::string>> &infoSS)
    {
        std::map<std::string, std::string> infoS;
        if (coverFlag)
        {
            dbOpera.clearDB(dbName);
        }
        if (infoSS.size() <= 0)
        {
            infoS["序号"] = "";
            infoS["参数"] = "";
            dbOpera.createDB(dbName, "序号", infoS);
        }
        for (const auto &infoS : infoSS)
        {
            dbOpera.writeRecord(dbName, "序号", infoS);
        }
        dbOpera.sortTable(dbName,"序号");
    }
    std::vector<std::map<std::string,std::string>> RWDb::getMethodParamMap(Method method){
        std::vector<std::map<std::string,std::string>> infoS;
        std::map<std::string,std::string> info;
        info["参数"]=method.param;
        info["序号"]=std::to_string(1);
        infoS.push_back(info);
        return infoS;
    }
    std::map<std::string,std::string> RWDb::getFlowMap(int index,FlowOfTask flow){
        std::map<std::string,std::string> infoS;
        infoS["序号"]=flow.indexStr;
        infoS["名称"]=flow.name;
        infoS["类型"]=flow.type;
        infoS["创建时间"]=flow.createTime;
        infoS["DBName"]=flow.dbName;
        return infoS;
    }
    std::map<std::string,std::string> RWDb::getMethodMap(int index,Method method){
        std::map<std::string,std::string> infoS;
        infoS["序号"]=method.indexStr;
        infoS["名称"]=method.name;
        infoS["类型"]=method.type;
        infoS["创建时间"]=method.createTime;
        infoS["DBName"]=method.dbName;
        infoS["参数"]=method.param;
        return infoS;
    }

    std::map<std::string,std::string> RWDb::getTaskMap(int index,Task taskInfo){
        std::map<std::string, std::string> infoS;
        infoS["序号"] = std::to_string(index);
        infoS["样品名称"] = taskInfo.sampleName;
        infoS["流程"] = taskInfo.testFlow;
        infoS["方法"] = taskInfo.testMethod;
        infoS["状态"]=taskInfo.runStatus;
        infoS["通道"] = std::to_string(taskInfo.testChannel);
        infoS["样品总量"] = std::to_string(taskInfo.content);

        #ifdef ENABLE_CAS6
        infoS["批号"]=taskInfo.batchNo;
        infoS["样品编号"]=taskInfo.serailNo;
        infoS["盘位"]=std::to_string(taskInfo.circleNo);
        infoS["检测状态"]=taskInfo.detectStatus;
        infoS["空白"]=taskInfo.blank;
        infoS["平衡策略"]=std::to_string(taskInfo.sampleDetectStrategy);
        #else
        infoS["样品来源"]=taskInfo.sampleInput;
        infoS["取样方式"]=taskInfo.getSampleMethod;
        infoS["目标成分"] = taskInfo.targetElement;
        infoS["单位"] = taskInfo.unit;
        infoS["condition1"] = taskInfo.standard1Condition;
        infoS["value1"] = std::to_string(taskInfo.standard1);
        infoS["condition2"] = taskInfo.standard2Condition;
        infoS["value2"] = std::to_string(taskInfo.standard2);
        infoS["工作模式"] = taskWorkingMode[taskInfo.workingMode];
        infoS["间隔时间"] = taskInterval[taskInfo.interval];
        infoS["取样管路"] = (taskInfo.getSamplePump);
        #endif
        return infoS;
    }
    std::vector<std::map<std::string,std::string>> RWDb::getFlowStepMap(FlowOfTask flow){
        std::vector<std::map<std::string,std::string>> infoS;
        for (auto step : flow.steps){
            std::map<std::string,std::string> info;
            info["序号"]=std::to_string(step.index);
            info["名称"]=step.name;
            info["参数"]=step.param;
            infoS.push_back(info);
        }
        return infoS;
    }

    FlowOfTask RWDb::readFlowOfTask(std::string dbName)
    {
        FlowOfTask flow;
        std::vector<std::map<std::string, std::string>> fillContent;
        std::map<std::string, std::string> info;
        info["序号"] = "";
        info["名称"] = "";
        info["参数"] = "";
        fillContent = readRecord(dbName, info);
        for (auto content : fillContent)
        {
            StepOfFlow step;
            step.index = std::stoi(content["序号"]);
            step.name = content["名称"];
            step.param = content["参数"];
            flow.steps.push_back(step);
        }
        return flow;
    }
    void RWDb::writeFlowManageRecord(const std::map<std::string, std::string> &infoS)
    {
        dbOpera.writeRecord(FLOWMANAGEDBNAME, "序号", infoS);
        dbOpera.sortTable(FLOWMANAGEDBNAME,"序号");
    }
    void RWDb::writeMethodManageRecord(const std::map<std::string, std::string> &infoS)
    {
        dbOpera.writeRecord(METHODMANAGENAME, "序号", infoS);
        dbOpera.sortTable(METHODMANAGENAME,"序号");
    }

    template <typename K, typename V>
    std::pair<bool, K> findKeyByValue(const std::map<K, V> &m, const V &value)
    {
        for (const auto &[key, val] : m)
        {
            if (val == value)
            {
                return {true, key};
            }
        }
        return {false, K()};
    }

    Task RWDb::getTaskSFromMap(std::map<std::string, std::string> infoS)
    {
        Task taskInfo;
        taskInfo.sampleName = infoS["样品名称"];
        taskInfo.testFlow = infoS["流程"];
        taskInfo.testMethod = infoS["方法"];
        taskInfo.runStatus = infoS["状态"];
        taskInfo.testChannel = std::stoi(infoS["通道"]);
        taskInfo.content = std::stod(infoS["样品总量"]);
        #ifdef ENABLE_CAS6
        taskInfo.batchNo=infoS["批号"];
        taskInfo.circleNo=std::atoi(infoS["盘位"].c_str());
        taskInfo.serailNo=infoS["样品编号"];
        taskInfo.detectStatus=infoS["检测状态"];
        taskInfo.blank=infoS["空白"];
        taskInfo.sampleDetectStrategy=std::atoi(infoS["平衡策略"].c_str());
        #else
        taskInfo.sampleInput = infoS["样品来源"];
        taskInfo.getSampleMethod = infoS["取样方式"];
        taskInfo.getSamplePump = infoS["取样管路"];
        taskInfo.targetElement = infoS["目标成分"];
        taskInfo.unit = infoS["单位"];
        taskInfo.standard1Condition = infoS["condition1"];
        taskInfo.standard1 = std::stod(infoS["value1"]);
        taskInfo.standard2Condition = infoS["condition2"];
        taskInfo.standard2 = std::stod(infoS["value2"]);
        auto result = findKeyByValue(taskWorkingMode, infoS["工作模式"]);
        if (result.first)
        {
            taskInfo.workingMode = result.second;
        }
        auto result1 = findKeyByValue(taskInterval, infoS["间隔时间"]);
        if (result1.first)
            taskInfo.interval = result1.second;
        #endif
        return taskInfo;
    }

    std::vector<std::string> RWDb::getAllAuditLogTables(){
        std::vector<std::string> tableNames=logOpera.getAllTables();
        for (int i=0;i<int(tableNames.size());i++){
            if (tableNames[i].find("AuditTrailLog") == std::string::npos){
                tableNames.erase(tableNames.begin()+i);
                i--;
            }
        }
        return tableNames;
    }
    std::vector<std::map<std::string,std::string>> RWDb::readAuditTrailLog(const std::string &tableName){
        std::map<std::string,std::string> info;
        info["lastAuditTrailDB"]="";
        logOpera.readSingleInfo(AUDITTRAILDBRECORD,info);

        std::map<std::string,std::string> infoS = {
            {"Operator",""},
            {"Time",""},
            {"LogContent",""}
        };
        std::string readTableName="";
        if (tableName != ""){
            readTableName = tableName;
        } else {
            readTableName = info["lastAuditTrailDB"];
        }
        return logOpera.readRecord(readTableName, infoS);
    }
    std::vector<std::map<std::string, std::string>> RWDb::readRecord(std::string dbName, std::map<std::string, std::string> &infoS)
    {
        return dbOpera.readRecord(dbName, infoS);
    }
    void RWDb::readSingleInfo(std::string dbName, std::map<std::string, std::string> &infoS)
    {
        dbOpera.readSingleInfo(dbName, infoS);
    }
    std::string RWDb::getUserInfoLastCount(){
        std::vector<std::map<std::string, std::string>> fillContent;
        fillContent = readUserInfo();
        std::string lastCount;
        int count = dbOpera.countOfTable(USERINFODBNAME);
        bool flag=false;
        for (int i=0;i<int(fillContent.size());i++){
            int curIndex= std::atoi(fillContent[i]["UserNo"].c_str());
            if (curIndex > count){
                flag=true;
                count = curIndex;
            }
        }
        std::string strcount;
        std::ostringstream oss;
        oss << std::setw(3) << std::setfill('0') << count+1;
        return oss.str();
    }
    void RWDb::insertTaskRunInfo(const std::string &startTime,const std::string &tableName)
    {
        std::string loginName=readLoginName();
        std::vector<std::map<std::string, std::string>> taskInfos = readTaskInfo(tableName); //  HgOnlinePlatformModule::getTaskSeqName());
        std::map<std::string, std::string> infoS;
        for (auto taskInfo : taskInfos)
        {
            infoS["序号"] = std::to_string(dataOpera.countOfTable(TASKRUNRECORDINFODBNAME) + 1);
            infoS["时间"] = startTime;
            infoS["通道"] = taskInfo["通道"];
            infoS["样品池"] = taskInfo["样品来源"];
            infoS["目标成分"] = taskInfo["目标成分"];
            infoS["流程"] = taskInfo["流程"];
            infoS["方法"] = taskInfo["方法"];
            infoS["检测器"] = "";
            infoS["滴定剂"] = "";
            infoS["反应液"] = "";
            infoS["溶剂"] = "";
            infoS["分析时长"] = "0";
            infoS["运行状态"] = "0";
            infoS["测试含量"] = "0";
            infoS["操作员"] = loginName == "" ? "admin" : loginName;
            infoS["DBName"] = "Data" + startTime;
            dataOpera.writeRecord(TASKRUNRECORDINFODBNAME,
                                        "序号", infoS);
        }
        dbOpera.sortTable(TASKRUNRECORDINFODBNAME,"序号");
    }
    std::vector<std::map<std::string, std::string>> RWDb::getTaskRunInfo()
    {
        std::vector<std::map<std::string, std::string>> infoS;
        std::map<std::string, std::string> info;
        #ifdef ENABLE_CAS6
        info["序号"] = "";
        info["时间"] = "";
        info["通道"] = "";
        info["样品池"] = "";
        info["目标成分"] = "";
        info["流程"] = "";
        info["方法"] = "";
        info["检测器"] = "";
        info["滴定剂"] = "";
        info["反应液"] = "";
        info["溶剂"] = "";
        info["分析时长"] = "";
        info["运行状态"] = "";
        info["测试含量"] = "";
        info["操作员"] = "";
        info["DBName"] = "";
        infoS = dataOpera.readRecord(TASKRUNRECORDINFODBNAME, info);
        #else
        info["序号"] = "";
        info["时间"] = "";
        info["通道"] = "";
        info["样品池"] = "";
        info["目标成分"] = "";
        info["流程"] = "";
        info["方法"] = "";
        info["检测器"] = "";
        info["滴定剂"] = "";
        info["反应液"] = "";
        info["溶剂"] = "";
        info["分析时长"] = "";
        info["运行状态"] = "";
        info["测试含量"] = "";
        info["操作员"] = "";
        info["DBName"] = "";
        infoS = dataOpera.readRecord(TASKRUNRECORDINFODBNAME, info);
        #endif
        return infoS;
    }

void RWDb::setTaskRunRecordDataDB(const std::string &dbName){
    std::map<std::string,std::string> mapNames;
    mapNames["curDB"]="Data"+dbName;
    recordSingleInfo(TASKRUNSTATUSNAME,mapNames);
}

void RWDb::setTaskRunFlag(std::string flag){
    std::map<std::string,std::string> mapNames;
    mapNames["start"]=flag;
    recordSingleInfo(TASKRUNSTATUSNAME,mapNames);
}
void RWDb::resetTaskRunFlag(){
    std::map<std::string,std::string> mapNames;
    mapNames["start"]="false";
    recordSingleInfo(TASKRUNSTATUSNAME,mapNames);
}
void RWDb::resetTaskRunStatus(const std::string &tableName)
{
    std::vector<std::map<std::string, std::string>> fillTaskContent = readTaskInfo(tableName); //HgOnlinePlatformModule::getTaskSeqName());
    for (int index = 0; index < int(fillTaskContent.size()); index++)
    {
        fillTaskContent[index]["状态"] = "";
    }
    writeTaskRecord(true, tableName,/*HgOnlinePlatformModule::getTaskSeqName(),*/fillTaskContent);
}
std::string RWDb::getTaskRunFlag(){
    std::map<std::string,std::string> mapNames;
    mapNames["start"]="";
    readSingleInfo(TASKRUNSTATUSNAME,mapNames);
    return mapNames["start"];
}

std::vector<std::string> RWDb::getFlowNames(){
    std::vector<std::map<std::string,std::string>> fillContent=readFlowInfo();
    std::vector<std::string> names;
    for (int i=0;i<int(fillContent.size());i++){    
        names.push_back(fillContent[i]["序号"]+"_"+fillContent[i]["名称"]);
    }
    return names;
}
std::string RWDb::getTaskRunRecordDataDB(){
    std::map<std::string,std::string> mapNames;
    mapNames["curDB"]="";
    readSingleInfo(TASKRUNSTATUSNAME,mapNames);
    return mapNames["curDB"];
}
    std::string RWDb::decodeMethodDBName(const std::string &methodName){
        std::vector<std::map<std::string, std::string>> fillContent = readMethodInfo();
        std::string dbName = "";
        for (int i = 0; i < int(fillContent.size()); i++)
        {
            std::string tempName = fillContent[i]["序号"] + "_" + fillContent[i]["名称"];
            if (tempName == methodName)
            {
                dbName = fillContent[i]["DBName"];
                break;
            }
        }
        return dbName;
    }
    std::string RWDb::decodeFlowDBName(const std::string &flowName)
    {
        // first find flow
        std::vector<std::map<std::string, std::string>> fillContent = readFlowInfo();
        std::string dbName = "";
        for (int i = 0; i < int(fillContent.size()); i++)
        {
            std::string tempName = fillContent[i]["序号"] + "_" + fillContent[i]["名称"];
            if (tempName == flowName)
            {
                dbName = fillContent[i]["DBName"];
                break;
            }
        }
        return dbName;
    }

    std::vector<std::map<std::string, std::string>> RWDb::readUserInfo()
    {
        std::vector<std::map<std::string, std::string>> fillContent;
        std::map<std::string, std::string> infoS;
        for (int i = 0; i < int(userInfoName.size()); i++)
        {
            infoS[userInfoName[i]] = "";
        }
        fillContent = readRecord(USERINFODBNAME, infoS);
        return fillContent;
    }
    std::vector<std::map<std::string,std::string>> RWDb::readUserGroupInfo(){
        std::vector<std::map<std::string, std::string>> fillContent;
        std::map<std::string, std::string> infoS;
        for (int i = 0; i < int(userGroupInfoName.size()); i++)
        {
            infoS[userGroupInfoName[i]] = "";
        }
        fillContent = readRecord(USERGROUPDBNAME,infoS);
        return fillContent;
    }
    void RWDb::deleteAuditTrail(){
        std::map<std::string,std::string> info;
        info["lastAuditTrailDB"]="";
        logOpera.recordSingleInfo(AUDITTRAILDBRECORD,info);
        std::vector<std::string> tableNames = getAllAuditLogTables();
        for (int i=0;i<int(tableNames.size());i++){
            logOpera.deleteDB(tableNames[i]);
        }
    }

    std::vector<std::map<std::string,std::string>> RWDb::searchAllAuditTrailLogs(const std::string &keyword, const HGExactTime &timeFrom, const HGExactTime &timeTo, int page, int pageSize){
        std::vector<std::map<std::string,std::string>> allResults;
        std::vector<std::string> tableNames = getAllAuditLogTables();
        
        // Sort tables by name (which includes date) in descending order to search recent tables first
        std::sort(tableNames.begin(), tableNames.end(), [](const std::string &a, const std::string &b){
            return a > b;
        });
        
        // Calculate offset and limit
        int offset = (page - 1) * pageSize;
        int remaining = pageSize;
        int totalSkipped = 0;
        
        for (const std::string &tableName : tableNames){
            // Build SQL query with time range and keyword filters
            std::string sql = "SELECT * FROM " + tableName + " WHERE 1=1";
            
            // Add time range filter
            std::string timeFromStr = std::to_string(timeFrom.tm_year) + "-" + 
                                     (timeFrom.tm_mon < 10 ? "0" : "") + std::to_string(timeFrom.tm_mon) + "-" + 
                                     (timeFrom.tm_mday < 10 ? "0" : "") + std::to_string(timeFrom.tm_mday) + " " +
                                     (timeFrom.tm_hour < 10 ? "0" : "") + std::to_string(timeFrom.tm_hour) + ":" +
                                     (timeFrom.tm_min < 10 ? "0" : "") + std::to_string(timeFrom.tm_min) + ":" +
                                     (timeFrom.tm_sec < 10 ? "0" : "") + std::to_string(timeFrom.tm_sec);
            
            std::string timeToStr = std::to_string(timeTo.tm_year) + "-" + 
                                   (timeTo.tm_mon < 10 ? "0" : "") + std::to_string(timeTo.tm_mon) + "-" + 
                                   (timeTo.tm_mday < 10 ? "0" : "") + std::to_string(timeTo.tm_mday) + " " +
                                   (timeTo.tm_hour < 10 ? "0" : "") + std::to_string(timeTo.tm_hour) + ":" +
                                   (timeTo.tm_min < 10 ? "0" : "") + std::to_string(timeTo.tm_min) + ":" +
                                   (timeTo.tm_sec < 10 ? "0" : "") + std::to_string(timeTo.tm_sec);
            
            sql += " AND Time >= '" + timeFromStr + "' AND Time <= '" + timeToStr + "'";
            
            // Add keyword filter using FTS if available
            if (!keyword.empty()){
                std::string ftsTableName = tableName + "_fts";
                sql += " AND rowid IN (SELECT rowid FROM " + ftsTableName + " WHERE " + ftsTableName + " MATCH '" + keyword + "')";
            }
            
            // Add order by
            sql += " ORDER BY Time DESC";
            
            // Get count of results in this table
            std::string countSql = "SELECT COUNT(*) as count FROM " + tableName + " WHERE 1=1";
            countSql += " AND Time >= '" + timeFromStr + "' AND Time <= '" + timeToStr + "'";
            if (!keyword.empty()){
                std::string ftsTableName = tableName + "_fts";
                countSql += " AND rowid IN (SELECT rowid FROM " + ftsTableName + " WHERE " + ftsTableName + " MATCH '" + keyword + "')";
            }
            
            std::vector<std::map<std::string,std::string>> countResults;
            logOpera.readData(countSql, countResults);
            int tableCount = 0;
            if (!countResults.empty()){
                tableCount = std::stoi(countResults[0]["count"]);
            }
            
            // Skip this table if we haven't reached the offset yet
            if (totalSkipped + tableCount <= offset){
                totalSkipped += tableCount;
                continue;
            }
            
            // Calculate how many records to skip in this table
            int skipInTable = offset - totalSkipped;
            totalSkipped += tableCount;
            
            // Add LIMIT and OFFSET to the query
            sql += " LIMIT " + std::to_string(remaining) + " OFFSET " + std::to_string(skipInTable);
            
            // Execute query and get results
            std::vector<std::map<std::string,std::string>> tableResults;
            logOpera.readData(sql, tableResults);
            
            // Add results to allResults
            for (const auto &record : tableResults){
                if (remaining <= 0) break;
                allResults.push_back(record);
                remaining--;
            }
            
            if (remaining <= 0) break;
        }
        
        return allResults;
    }

    int RWDb::searchAllAuditTrailLogsCount(const std::string &keyword, const HGExactTime &timeFrom, const HGExactTime &timeTo){
        int totalCount = 0;
        std::vector<std::string> tableNames = getAllAuditLogTables();
        
        for (const std::string &tableName : tableNames){
            // Build SQL query with time range and keyword filters
            std::string sql = "SELECT COUNT(*) as count FROM " + tableName + " WHERE 1=1";
            
            // Add time range filter
            std::string timeFromStr = std::to_string(timeFrom.tm_year) + "-" + 
                                     (timeFrom.tm_mon < 10 ? "0" : "") + std::to_string(timeFrom.tm_mon) + "-" + 
                                     (timeFrom.tm_mday < 10 ? "0" : "") + std::to_string(timeFrom.tm_mday) + " " +
                                     (timeFrom.tm_hour < 10 ? "0" : "") + std::to_string(timeFrom.tm_hour) + ":" +
                                     (timeFrom.tm_min < 10 ? "0" : "") + std::to_string(timeFrom.tm_min) + ":" +
                                     (timeFrom.tm_sec < 10 ? "0" : "") + std::to_string(timeFrom.tm_sec);
            
            std::string timeToStr = std::to_string(timeTo.tm_year) + "-" + 
                                   (timeTo.tm_mon < 10 ? "0" : "") + std::to_string(timeTo.tm_mon) + "-" + 
                                   (timeTo.tm_mday < 10 ? "0" : "") + std::to_string(timeTo.tm_mday) + " " +
                                   (timeTo.tm_hour < 10 ? "0" : "") + std::to_string(timeTo.tm_hour) + ":" +
                                   (timeTo.tm_min < 10 ? "0" : "") + std::to_string(timeTo.tm_min) + ":" +
                                   (timeTo.tm_sec < 10 ? "0" : "") + std::to_string(timeTo.tm_sec);
            
            sql += " AND Time >= '" + timeFromStr + "' AND Time <= '" + timeToStr + "'";
            
            // Add keyword filter using FTS if available
            if (!keyword.empty()){
                std::string ftsTableName = tableName + "_fts";
                sql += " AND rowid IN (SELECT rowid FROM " + ftsTableName + " WHERE " + ftsTableName + " MATCH '" + keyword + "')";
            }
            
            // Execute query and get count
            std::vector<std::map<std::string,std::string>> results;
            logOpera.readData(sql, results);
            if (!results.empty()){
                totalCount += std::stoi(results[0]["count"]);
            }
        }
        
        return totalCount;
    }

    void RWDb::writeAuditTrailLog(const std::string &logContent)
    {
        std::map<std::string,std::string> noLoginRecordInfo;
        noLoginRecordInfo["track"] = "";
        noLoginRecordInfo["debug"]="";
        readSingleInfo(SYSTEMINFODBNAME,noLoginRecordInfo);
        // if (noLoginRecordInfo["track"] != "true"&&noLoginRecordInfo["debug"]!="true") return;
        std::map<std::string, std::string> infoS = {
            {"Operator", readLoginName()},
            {"Time", HGExactTime::currentTime().toStringFromYearToSecAndZone()},
            {"LogContent", logContent}};

        std::map<std::string,std::string> info;
        info["lastAuditTrailDB"]="";
        logOpera.readSingleInfo(AUDITTRAILDBRECORD,info);
        std::string dbName="";
        bool isNewTable = false;
        if (info["lastAuditTrailDB"] == "")
        {
            HGExactTime curTime = HGExactTime::currentTime();
            dbName = "AuditTrailLog_" + curTime.toStringFromYearToDay();
            printf("blank: %s\n",dbName.c_str());
            isNewTable = true;
        }
        else
        {
            int count = logOpera.countOfTable(info["lastAuditTrailDB"]);
            if (count >= 3000)
            {
                HGExactTime curTime = HGExactTime::currentTime();
                dbName = "AuditTrailLog_" + curTime.toStringFromYearToDay();
                printf(">=3000: %s\n",dbName.c_str());
                isNewTable = true;
            }
            else
            {
                dbName = info["lastAuditTrailDB"];
                printf("<3000: %s\n",dbName.c_str());
            }
        }
        logOpera.writeRecord(dbName, "Time", infoS);
        
        // Add indexes to the table if it's a new table
        if (isNewTable) {
            // Add index on Time column for time range queries
            logOpera.writeData("CREATE INDEX IF NOT EXISTS idx_" + dbName + "_time ON " + dbName + " (Time)");
            // Add index on Operator column for operator searches
            logOpera.writeData("CREATE INDEX IF NOT EXISTS idx_" + dbName + "_operator ON " + dbName + " (Operator)");
            
            // Create FTS virtual table for full-text search
            std::string ftsTableName = dbName + "_fts";
            logOpera.writeData("CREATE VIRTUAL TABLE IF NOT EXISTS " + ftsTableName + " USING FTS5(LogContent, Operator, Time)");
            
            // Create trigger to keep FTS table in sync
            logOpera.writeData("CREATE TRIGGER IF NOT EXISTS " + dbName + "_ai AFTER INSERT ON " + dbName + " BEGIN INSERT INTO " + ftsTableName + "(LogContent, Operator, Time) VALUES (new.LogContent, new.Operator, new.Time); END");
            logOpera.writeData("CREATE TRIGGER IF NOT EXISTS " + dbName + "_ad AFTER DELETE ON " + dbName + " BEGIN DELETE FROM " + ftsTableName + " WHERE rowid = old.rowid; END");
            logOpera.writeData("CREATE TRIGGER IF NOT EXISTS " + dbName + "_au AFTER UPDATE ON " + dbName + " BEGIN UPDATE " + ftsTableName + " SET LogContent = new.LogContent, Operator = new.Operator, Time = new.Time WHERE rowid = old.rowid; END");
        }
        info["lastAuditTrailDB"]=dbName;
        logOpera.recordSingleInfo(AUDITTRAILDBRECORD,info);
    }
    void RWDb::writeSerialPortInfo(const std::vector<std::string> &serialPorts)
    {
        std::map<std::string, std::string> info;
        dbOpera.clearDB(SERIALPORTDBNAME);
        std::string keyName = "序号";
        for (int i = 0; i < int(serialPorts.size()); i++)
        {
            info["序号"] = std::to_string(i + 1);
            info["名称"] = serialPorts[i];
            dbOpera.writeRecord(SERIALPORTDBNAME, keyName, info);
        }
    }
    void RWDb::writeUserInfo(const std::map<std::string,std::string> &infoS){
        dbOpera.writeRecord(USERINFODBNAME, "UserNo",infoS);
        dbOpera.sortTable(USERINFODBNAME,"UserNo");
    }
    void RWDb::writeUserGroupInfo(const std::map<std::string,std::string> &infoS){
        dbOpera.writeRecord(USERGROUPDBNAME, "GroupName",infoS);
        dbOpera.sortTable(USERGROUPDBNAME,"GroupName");
    }
    void RWDb::deleteUserGroupInfo(const std::string &value){
        dbOpera.deleteRecord(USERGROUPDBNAME,"GroupName",value);
    }
    std::string RWDb::getLastLoginTime()
    {
        std::map<std::string, std::string> noLoginRecordInfo;
        noLoginRecordInfo["lastLoginTime"] = "";
        readSingleInfo(SYSTEMINFODBNAME,noLoginRecordInfo);
        return noLoginRecordInfo["lastLoginTime"];
    }
    bool RWDb::isLoginRightTime()
    {
        std::string lastLoginTime = getLastLoginTime();
        if (lastLoginTime == "")
            return true;
        HGExactTime curTimer = HGExactTime::currentTime();
        HGExactTime deadlineTimer = HGExactTime::currentTime();
        TIME_STRUECT tmS;
        decodeStandardTime(lastLoginTime, tmS);
        deadlineTimer.tm_year = tmS.year; 
        deadlineTimer.tm_mon = tmS.month; 
        deadlineTimer.tm_mday = tmS.day; 
        if (curTimer < deadlineTimer)
        {
            return false;
        }
        return true;
    }

void RWDb::clearFlowManageRecord(){
    clearDB(FLOWMANAGEDBNAME);
}
void RWDb::clearMethodManageRecord(){
    clearDB(METHODMANAGENAME);
}
    void RWDb::deleteRecord(std::string dbName, std::string deleteKey, std::string deleteValue)
    {
        dbOpera.deleteRecord(dbName, deleteKey, deleteValue);
    }
    std::vector<std::string> RWDb::readSerialPortInfo(){
        std::map<std::string,std::string> info;
        info["序号"]="";
        info["名称"]="";
        std::vector<std::map<std::string,std::string>> contents;
        contents=dbOpera.readRecord(SERIALPORTDBNAME,info);
        std::vector<std::string> ports;
        for (auto port : contents){
            ports.push_back(port["名称"]);
        }
        return ports;
    }
    void RWDb::deleteDB(const std::string &dbName)
    {
        dbOpera.deleteDB(dbName);
    }
    std::vector<std::string> RWDb::getAllTables(std::string key)
    {
        std::vector<std::string> returnNames;
        std::vector<std::string> names = dbOpera.getAllTables();
        for (const auto &name : names)
        {
            if (name.find(key) != std::string::npos)
            {
                returnNames.push_back(name);
            }
        }
        return returnNames;
    }
    std::vector<std::map<std::string,std::string>> RWDb::readChannelInfo(){
        std::vector<std::map<std::string,std::string>> fillContent;
        std::map<std::string,std::string> infoS;
        for (int i=0;i<int(channelInfoName.size());i++){
            infoS[channelInfoName[i]]="";
        }
        fillContent=readRecord(CHANNELMANAGENAME,infoS);
        return fillContent;
    }

    void RWDb::writeChannelManageRecord(const std::map<std::string, std::string> &infoS)
    {
        dbOpera.writeRecord(CHANNELMANAGENAME, "序号", infoS);
        dbOpera.sortTable(CHANNELMANAGENAME,"序号");
    }

std::map<std::string,std::string> RWDb::getChannelMap(int index,Channel channel){
    std::map<std::string,std::string> infoS;
    infoS["序号"]=std::to_string(index);
    infoS["通道"]=std::to_string(channel.channel);
    infoS["检测器"]="";
    infoS["检测方法"]="";
    infoS["DBName"]=ChannelDBName+std::string("_")+infoS["序号"];
    for (auto content : channel.modules){
        if (content.typeName=="检测器"){
            infoS["检测器"]=content.name;
            infoS["检测方法"]=content.param;
        }
    }
    return infoS;
}

void RWDb::clearChannelManageRecord(){
    clearDB(CHANNELMANAGENAME);
}

std::vector<ModuleOfChannel> RWDb::readModulesParam(std::string dbName){
    std::vector<ModuleOfChannel> modules;
    std::vector<std::map<std::string,std::string>> fillContent;
    std::map<std::string,std::string> info;
    info["序号"]="";
    info["typeName"]="";
    info["名称"]="";
    info["参数"]="";
    fillContent=readRecord(dbName,info);
    if (fillContent.size()>0){
        for (auto infoS : fillContent){
            ModuleOfChannel moduleOfChannel;
            if (infoS["序号"]!="") moduleOfChannel.index=std::stoi(infoS["序号"]);
            moduleOfChannel.name=infoS["名称"];
            moduleOfChannel.typeName=infoS["typeName"];
            moduleOfChannel.param=infoS["参数"];
            modules.push_back(moduleOfChannel);
        }
    }
    return modules;
}
std::vector<REAGENT> RWDb::readReagentInfo(const std::string &reagentTableName){
    std::vector<REAGENT> reagents;
    std::vector<std::map<std::string,std::string>> fillContent;
    std::map<std::string,std::string> infoS;
    for (int i=0;i<int(reagentInfoName.size());i++){
        infoS[reagentInfoName[i]]="";
    }
    fillContent=readRecord(/*HgOnlinePlatformModule::getReagentName()*/reagentTableName,infoS);
    for(int i=0;i<int(fillContent.size());i++){
        REAGENT reagent;
        reagent.index=std::stoi(fillContent[i]["序号"]);
        reagent.type=fillContent[i]["试剂类别"];
        reagent.serialNumber=fillContent[i]["试剂编码"];
        reagent.name=fillContent[i]["试剂名称"];
        reagent.concertraction=fillContent[i]["滴定度浓度"];
        reagent.calibrationDate=fillContent[i]["标定日期"];
        reagent.calibrationCircle=fillContent[i]["标定周期"];
        reagent.reagentRemainAmount=fillContent[i]["试剂余量"];
        reagent.currentState=fillContent[i]["当前状态"];
        reagent.dbName=fillContent[i]["DBName"];
        std::vector<std::map<std::string,std::string>> contents;
        infoS.clear();
        for (int j=0;j<int(reagentLinkDeviceName.size());j++){
            infoS[reagentLinkDeviceName[j]]="";
        }
        contents=readRecord(reagent.dbName,infoS);
        for (int j=0;j<int(contents.size());j++){
            reagent.linkDevices[j].index=std::stoi(contents[j]["序号"]);
            reagent.linkDevices[j].name=contents[j]["指标设备单元"];
            reagent.linkDevices[j].interfaceName=contents[j]["接口"];
            reagent.linkDevices[j].linkState=contents[j]["联机"];
            reagent.linkDevices[j].channel=contents[j]["当前通道"];
            reagent.linkDevices[j].choice=contents[j]["选择"];
        }
        reagents.push_back(reagent);
    }
    return reagents;
}

std::map<std::string,std::string> RWDb::getMapFromReagent(const REAGENT &info){
    std::map<std::string,std::string> infoMap;
    infoMap["序号"]=std::to_string(info.index);
    infoMap["试剂类别"]=info.type;
    infoMap["试剂编码"]=info.serialNumber;
    infoMap["试剂名称"]=info.name;
    infoMap["滴定度浓度"]=info.concertraction;
    infoMap["标定日期"]=info.calibrationDate;
    infoMap["标定周期"]=info.calibrationCircle;
    infoMap["试剂余量"]=info.reagentRemainAmount;
    infoMap["当前状态"]=info.currentState;
    infoMap["DBName"]=info.dbName;
    return infoMap;
}

REAGENT RWDb::getFromReagentMap(std::map<std::string,std::string> regentInfo,
    std::vector<std::map<std::string,std::string>> deviceInfos)
{
    REAGENT reagent;
    reagent.index=std::stoi(regentInfo["序号"]);
    reagent.type=regentInfo["试剂类别"];
    reagent.serialNumber=regentInfo["试剂编码"];
    reagent.name=regentInfo["试剂名称"];
    reagent.concertraction=regentInfo["滴定度浓度"];
    reagent.calibrationDate=regentInfo["标定日期"];
    reagent.calibrationCircle=regentInfo["标定周期"];
    reagent.reagentRemainAmount=regentInfo["试剂余量"];
    reagent.currentState=regentInfo["当前状态"];
    reagent.dbName=regentInfo["DBName"];
    for (int i=0;i<int(deviceInfos.size());i++){
        reagent.linkDevices[i].index=std::stoi(deviceInfos[i]["序号"]);
        reagent.linkDevices[i].name=deviceInfos[i]["指标设备单元"];
        reagent.linkDevices[i].interfaceName=deviceInfos[i]["接口"];
        reagent.linkDevices[i].linkState=deviceInfos[i]["联机"];
        reagent.linkDevices[i].channel=deviceInfos[i]["当前通道"];
        reagent.linkDevices[i].choice=deviceInfos[i]["选择"];
    }
    return reagent;
}

void RWDb::deleteAllChannelModuleDB(){
    std::vector<std::string> names=getAllTables(ChannelDBName);
    for (auto name:names){
        if (name==CHANNELMANAGENAME) continue;
        deleteDB(name);
    }
}
std::vector<std::map<std::string,std::string>> RWDb::getModulesMap(Channel info){
    std::vector<std::map<std::string,std::string>> infoS;
    for (auto module : info.modules){
        std::map<std::string,std::string> info;
        info["序号"]=std::to_string(module.index);
        info["typeName"]=module.typeName;
        info["名称"]=module.name;
        info["参数"]=module.param;
        infoS.push_back(info);
    }
    return infoS;
}


void RWDb::copyTable(const std::string& sourceDBName,
            const std::string &targetDbName,const std::string &tableName){
    if (targetDbName==(RWDb::readCurDirPath()+DB_PATH)){
        bool flag = dbOpera.copyTable(sourceDBName, tableName);
#if defined(_MSC_VER) || defined(WIN64) || defined(_WIN64) || defined(__WIN64__) || defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)

#else
        if (flag)
        {
            HGLog4Cplus::getLogInstance(LOG_PATH)->logout("copy table " + tableName + " success", LOGINFO);
        }
        else
        {
            HGLog4Cplus::getLogInstance(LOG_PATH)->logout("copy table " + tableName + " fail", LOGERROR);
        }
#endif
        std::vector<std::map<std::string, std::string>> fillContent;
        std::map<std::string, std::string> infoS;
        if (tableName.find("Flow_") != std::string::npos)
        {
            HGSaveDataToDB dbOperatorTemp;
            dbOperatorTemp.openDB(sourceDBName);
            for (int i = 0; i < int(flowInfoName.size()); i++)
            {
                infoS[flowInfoName[i]] = "";
            }
            fillContent = dbOperatorTemp.readRecord(FLOWMANAGEDBNAME, infoS);

            for (int i = 0; i < int(fillContent.size()); i++)
            {
                if (fillContent[i]["DBName"] == tableName)
                {
                    fillContent[i]["序号"] = std::to_string(readFlowInfo().size() + 1);
                    dbOpera.writeRecord(FLOWMANAGEDBNAME, "序号", fillContent[i]);
                }
            }

            dbOpera.sortTable(FLOWMANAGEDBNAME,"序号");
            dbOperatorTemp.closeDB();
        }else if (tableName.find("Channel_") != std::string::npos)
        {
            HGSaveDataToDB dbOperatorTemp;
            dbOperatorTemp.openDB(sourceDBName);
            for (int i=0;i<int(channelInfoName.size());i++){
                infoS[channelInfoName[i]]="";
            }
            
            fillContent = dbOperatorTemp.readRecord(CHANNELMANAGENAME, infoS);

            for (int i = 0; i < int(fillContent.size()); i++)
            {
                if (fillContent[i]["DBName"] == tableName)
                {
                    fillContent[i]["序号"] = std::to_string(readChannelInfo().size() + 1);
                    dbOpera.writeRecord(CHANNELMANAGENAME, "序号", fillContent[i]);
                }
            }
            dbOpera.sortTable(CHANNELMANAGENAME,"序号");
            dbOperatorTemp.closeDB();
        }else if (tableName.find("Method_") != std::string::npos)
        {
            HGSaveDataToDB dbOperatorTemp;
            dbOperatorTemp.openDB(sourceDBName);
            for (int i = 0; i < int(methodInfoName.size()); i++)
            {
                infoS[methodInfoName[i]] = "";
            }
            fillContent =dbOperatorTemp.readRecord(METHODMANAGENAME, infoS);
            for (int i = 0; i < int(fillContent.size()); i++)
            {
                if (fillContent[i]["DBName"] == tableName)
                {
                    fillContent[i]["序号"] = std::to_string(readMethodInfo().size() + 1);
                    dbOpera.writeRecord(METHODMANAGENAME, "序号", fillContent[i]);
                }
            }
            dbOpera.sortTable(METHODMANAGENAME,"序号");
            dbOperatorTemp.closeDB();
        }else if (tableName.find("Reagent_") != std::string::npos)
        {
            HGSaveDataToDB dbOperatorTemp;
            dbOperatorTemp.openDB(sourceDBName);
            std::vector<REAGENT> reagents;
            for (int i=0;i<int(reagentInfoName.size());i++){
                infoS[reagentInfoName[i]]="";
            }
            fillContent=dbOperatorTemp.readRecord(tableName,infoS);
            std::map<std::string,int> reagentDeviceTableNameCount;
            for (int i=0;i<int(fillContent.size());i++){
                reagentDeviceTableNameCount[fillContent[i]["DBName"]]=1;
            }
            dbOperatorTemp.closeDB();
            for (auto deviceTableName:reagentDeviceTableNameCount){
                bool flag=dbOpera.copyTable(sourceDBName,deviceTableName.first);
            }
        }
        else if (tableName.find("TaskSequence_") != std::string::npos)
        { // nothing
        }
    }
}

    void RWDb::writeReagentRecord(bool coverFlag, const std::string& tableName,const std::vector<REAGENT> &infoSS)
    {
        std::map<std::string, std::string> infoS;
        if (coverFlag)
        {
            dbOpera.clearDB(tableName/*HgOnlinePlatformModule::getReagentName()*/);
        }
        if (infoSS.size() <= 0)
        {
            for (int i = 0; i < int(reagentInfoName.size()); i++)
                infoS[reagentInfoName[i]] = "";
            dbOpera.writeRecord(tableName/*HgOnlinePlatformModule::getReagentName()*/, "序号", infoS);
            dbOpera.sortTable(tableName/*HgOnlinePlatformModule::getReagentName()*/,"序号");
            infoS.clear();
        }
        for (auto reagentInfo : infoSS)
        {
            infoS.clear();
            std::string deviceDBName = tableName/*HgOnlinePlatformModule::getReagentName()*/ + "_" + ReagentLinkDeviceDBName + std::to_string(reagentInfo.index);
            infoS["序号"] = std::to_string(reagentInfo.index);
            infoS["试剂类别"] = reagentInfo.type;
            infoS["试剂编码"] = reagentInfo.serialNumber;
            infoS["试剂名称"] = reagentInfo.name;
            infoS["滴定度浓度"] = reagentInfo.concertraction;
            infoS["标定日期"] = reagentInfo.calibrationDate;
            infoS["标定周期"] = reagentInfo.calibrationCircle;
            infoS["试剂余量"] = reagentInfo.reagentRemainAmount;
            infoS["当前状态"] = reagentInfo.currentState;
            infoS["DBName"] = deviceDBName;
            dbOpera.writeRecord(tableName/*HgOnlinePlatformModule::getReagentName()*/, "序号", infoS);
            dbOpera.sortTable(tableName/*HgOnlinePlatformModule::getReagentName()*/,"序号");
            infoS.clear();
            dbOpera.clearDB(deviceDBName);
            for (auto deviceS : reagentInfo.linkDevices)
            {
                infoS["序号"] = std::to_string(deviceS.second.index);
                infoS["指标设备单元"] = deviceS.second.name;
                infoS["接口"] = deviceS.second.interfaceName;
                infoS["联机"] = deviceS.second.linkState;
                infoS["当前通道"] = deviceS.second.channel;
                infoS["选择"] = deviceS.second.choice;
                dbOpera.writeRecord(deviceDBName, "序号", infoS);
            }
            dbOpera.sortTable(deviceDBName,"序号");
        }
    }


std::vector<std::map<std::string,std::string>> RWDb::readReagentDeviceInfo(std::string dbName){
    std::vector<std::map<std::string,std::string>> fillContent;
    std::map<std::string,std::string> info;
    for (int i=0;i<int(reagentLinkDeviceName.size());i++)
        info[reagentLinkDeviceName[i]]="";
    fillContent=readRecord(dbName,info);
    return fillContent;
}

    void RWDb::writeModulesRecord(std::string dbName, bool coverFlag, const std::vector<std::map<std::string, std::string>> &infoSS)
    {
        std::map<std::string, std::string> infoS;
        if (coverFlag)
        {
            dbOpera.clearDB(dbName);
        }
        if (infoSS.size() <= 0)
        {
            infoS["序号"] = "";
            infoS["typeName"] = "";
            infoS["名称"] = "";
            infoS["参数"] = "";
            dbOpera.createDB(dbName, "序号", infoS);
        }
        for (const auto &infoS : infoSS)
        {
            dbOpera.writeRecord(dbName, "序号", infoS);
        }
        dbOpera.sortTable(dbName,"序号");
    }


const std::vector<std::string> RWDb::getReagentNoEditName() {
    return reagentNoEditName;
}
const std::vector<std::string> RWDb::getReagentLinkDeviceNoEditName(){
    return reagentLinkDeviceNoEditName;
}

std::map<std::string,std::string> RWDb::getMapFromDevices(const ReagentLinkDevice &devices){
    ReagentLinkDevice device=devices;
    std::map<std::string,std::string> info;
    info["序号"]=std::to_string(device.index);
    info["指标设备单元"]=device.name;
    info["接口"]=device.interfaceName;
    info["联机"]=device.linkState;
    info["当前通道"]=device.channel;
    info["选择"]=device.choice;
    return info;
}
void RWDb::writeScannerInfo(const std::map<std::string,std::string> &info){
    dbOpera.recordSingleInfo(ScannerDBName, info);
}
    std::map<std::string,std::string> RWDb::readScannerInfo(){
        std::map<std::string,std::string> info;
        for (auto name:scannerInfoName){
            info[name]="";
        }
        dbOpera.readSingleInfo(ScannerDBName,info);
        return info;
    }
}