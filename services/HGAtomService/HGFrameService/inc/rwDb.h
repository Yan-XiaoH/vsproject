#ifndef RWDB_H
#define RWDB_H

#include "hgsavedatatodb.h"

#ifdef __cplusplus
extern "C" {
#endif
namespace HGMACHINE{
    typedef struct tagUserInfoS{
    std::string userno;
    std::string username;
    std::string userpwd;
    std::string authority;
    std::string department;
    tagUserInfoS(){
        username="";
        userpwd="";
        authority="";
        department="";
        userno="";
    }
    tagUserInfoS(std::string _name,
        std::string pwd,
        std::string _authority,
        std::string _depart,
        std::string _userno)
    {
        username=_name;
        userpwd=pwd;
        authority=_authority;
        department=_depart;
        userno=_userno;
    }
    bool operator==(const tagUserInfoS& obj) const{
        bool ret=false;
        if (username==obj.username &&
            userpwd==obj.userpwd &&
            authority==obj.authority &&
            department==obj.department &&
            userno==obj.userno)
        {
            ret=true;
        }
        return ret;
    }
    tagUserInfoS(const tagUserInfoS& obj){
        Copy(obj);
    }
    void Copy(const tagUserInfoS& obj){
        username=obj.username;
        userpwd=obj.userpwd;
        authority=obj.authority;
        department=obj.department;
        userno=obj.userno;
    }
    tagUserInfoS& operator=(const tagUserInfoS& obj){
        Copy(obj);
        return *this;
    }
    bool operator!=(const tagUserInfoS& obj)const
    {
        return (!(*this==obj));
    }
    tagUserInfoS* Clone() const
    {
        return (new tagUserInfoS(*this));
    }
} UserInfoS, *PUserInfoS;

    typedef struct tagDeviceInterface{
        std::string rootName;
        std::string name;
        std::string param;
        tagDeviceInterface(){
            Clear();
        }
        void Clear(){
            rootName="";
            name="";
            param="";
        }
        bool operator==(const tagDeviceInterface& obj) const{
            bool ret=false;
            if (rootName==obj.rootName &&
                name==obj.name &&
                param==obj.param)
            {
                ret=true;
            }
            return ret;
        }
        tagDeviceInterface(const tagDeviceInterface& obj){
            Copy(obj);
        }
        void Copy(const tagDeviceInterface& obj){
            rootName=obj.rootName;
            name=obj.name;
            param=obj.param;
        }
        tagDeviceInterface& operator=(const tagDeviceInterface& obj){
            Copy(obj);
            return *this;
        }
        bool operator!=(const tagDeviceInterface& obj)const
        {
            return (!(*this==obj));
        }
        tagDeviceInterface* Clone() const
        {
            return (new tagDeviceInterface(*this));
        }
    }DeviceInterface,*PDeviceInterface;
    typedef struct tagReagentLinkDevice{
        int index;
        std::string name;
        std::string interfaceName;
        std::string linkState;
        std::string channel;
        std::string choice;
        tagReagentLinkDevice(){
            Clear();
        }
        void Clear(){
            index=-1;
            interfaceName="";
            name="";
            linkState="";
            channel="";
            choice="";
        }
        bool operator==(const tagReagentLinkDevice& obj) const{
            bool ret=false;
            if (index==obj.index &&
                name==obj.name &&
                interfaceName==obj.interfaceName &&
                linkState==obj.linkState &&
                channel==obj.channel &&
                choice==obj.choice)
            {
                ret=true;
            }
            return ret;
        }
        tagReagentLinkDevice(const tagReagentLinkDevice& obj){
            Copy(obj);
        }
        void Copy(const tagReagentLinkDevice& obj){
            index=obj.index;
            name=obj.name;
            interfaceName=obj.interfaceName;
            linkState=obj.linkState;
            channel=obj.channel;
            choice=obj.choice;
        }
        tagReagentLinkDevice& operator=(const tagReagentLinkDevice& obj){
            Copy(obj);
            return *this;
        }
        bool operator!=(const tagReagentLinkDevice& obj)const
        {
            return (!(*this==obj));
        }
        tagReagentLinkDevice* Clone() const
        {
            return (new tagReagentLinkDevice(*this));
        }
    } ReagentLinkDevice,*PReagentLinkDevice;
    typedef struct tagReagent{
        int index;
        std::string type;
        std::string serialNumber;
        std::string name;
        std::string concertraction;
        std::string calibrationDate;
        std::string calibrationCircle;
        std::string reagentRemainAmount;
        std::string currentState;
        std::string dbName;
        std::map<int,ReagentLinkDevice> linkDevices;
        tagReagent(){
            Clear();
        }
        void Clear(){
            index=-1;
            name="";
            type="";
            serialNumber="";
            concertraction="";
            calibrationDate="";
            calibrationCircle="";
            reagentRemainAmount="";
            currentState="";
            dbName="";
            for(auto device : linkDevices) device.second.Clear();
        }
        bool operator==(const tagReagent& obj) const{
            bool ret=false;
            if (index==obj.index &&
                name==obj.name &&
                type==obj.type &&
                serialNumber==obj.serialNumber &&
                concertraction==obj.concertraction &&
                calibrationDate==obj.calibrationDate &&
                calibrationCircle==obj.calibrationCircle &&
                reagentRemainAmount==obj.reagentRemainAmount &&
                currentState==obj.currentState &&
                dbName==obj.dbName &&
                linkDevices.size()==obj.linkDevices.size() &&
                linkDevices==obj.linkDevices)
            {
                // std::map<int,ReagentLinkDevice>::iterator it=obj.linkDevices.begin();
                // std::map<int,ReagentLinkDevice>::iterator it1=linkDevices.begin();
                // for (;it!=obj.linkDevices.end(),it1!=linkDevices.end();it++,it1++) {
                //     if (it->second!=it1->second)
                //     {
                //         return false;
                //     }
                // }
                ret=true;
            }
            return ret;
        }
        tagReagent(const tagReagent& obj){
            Copy(obj);
        }
        void Copy(const tagReagent& obj){
            index=obj.index;
            name=obj.name;
            type=obj.type;
            serialNumber=obj.serialNumber;
            concertraction=obj.concertraction;
            calibrationDate=obj.calibrationDate;
            calibrationCircle=obj.calibrationCircle;
            reagentRemainAmount=obj.reagentRemainAmount;
            currentState=obj.currentState;
            dbName=obj.dbName;
            for (const auto &device : obj.linkDevices)
                linkDevices[device.first]=device.second;
        }
        tagReagent& operator=(const tagReagent& obj){
            Copy(obj);
            return *this;
        }
        bool operator!=(const tagReagent& obj)const
        {
            return (!(*this==obj));
        }
        tagReagent* Clone() const
        {
            return (new tagReagent(*this));
        }
    } REAGENT,*PREAGENT;
    typedef struct tagModuleOfChannel{
        int index;
        std::string name;
        std::string param;
        std::string nameOfSameModule;
        int indexOfSameModule;
        std::string typeName;
        tagModuleOfChannel(){
            Clear();
        }
        void Clear(){
            index=-1;
            name="";
            param="";
            typeName="";
            nameOfSameModule="";
            indexOfSameModule=-1;
        }
        bool operator==(const tagModuleOfChannel& obj) const{
            bool ret=false;
            if (index==obj.index &&
                name==obj.name &&
                typeName==obj.typeName &&
                param==obj.param &&
                nameOfSameModule==obj.nameOfSameModule &&
                indexOfSameModule==obj.indexOfSameModule)
            {
                ret=true;
            }
            return ret;
        }
        tagModuleOfChannel(const tagModuleOfChannel& obj){
            Copy(obj);
        }
        void Copy(const tagModuleOfChannel& obj){
            index=obj.index;
            name=obj.name;
            param=obj.param;
            typeName=obj.typeName;
            nameOfSameModule=obj.nameOfSameModule;
            indexOfSameModule=obj.indexOfSameModule;
        }
        tagModuleOfChannel& operator=(const tagModuleOfChannel& obj){
            Copy(obj);
            return *this;
        }
        bool operator!=(const tagModuleOfChannel& obj)const
        {
            return (!(*this==obj));
        }
        tagModuleOfChannel* Clone() const
        {
            return (new tagModuleOfChannel(*this));
        }
    } ModuleOfChannel, *PModuleOfChannel;
    typedef struct tagChannel{
        int channel;
        std::string dbName;
        std::vector<ModuleOfChannel> modules;
        tagChannel(){
            Clear();
        }
        void Clear(){
            channel=-1;
            dbName="";
            modules.clear();
        }
        bool operator==(const tagChannel& obj) const{
            bool ret=false;
            if (channel==obj.channel &&
                dbName==obj.dbName &&
                modules==obj.modules)
            {
                ret=true;
            }
            return ret;
        }
        tagChannel(const tagChannel& obj){
            Copy(obj);
        }
        void Copy(const tagChannel& obj){
            channel=obj.channel;
            modules.clear();
            for (int i=0;i<int(obj.modules.size());i++){
                modules.push_back(obj.modules[i]);
            }
            dbName=obj.dbName;
        }
        tagChannel& operator=(const tagChannel& obj){
            Copy(obj);
            return *this;
        }
        bool operator!=(const tagChannel& obj)const
        {
            return (!(*this==obj));
        }
        tagChannel* Clone() const
        {
            return (new tagChannel(*this));
        }
    }Channel,*PChannel;
    typedef struct tagMethod{
        std::string indexStr;
        std::string name;
        std::string type;
        std::string createTime;
        std::string dbName;
        std::string param;
        tagMethod(){
            Clear();
        }
        void Clear(){
            indexStr="";
            name="";
            type="";
            createTime="";
            dbName="";
            param="";
        }
        bool operator==(const tagMethod& obj) const{
            bool ret=false;
            if (indexStr==obj.indexStr &&
                name==obj.name &&
                type==obj.type &&
                createTime==obj.createTime &&
                dbName==obj.dbName && 
                param==obj.param)
            {
                ret=true;
            }
            return ret;
        }
        tagMethod(const tagMethod& obj){
            Copy(obj);
        }
        void Copy(const tagMethod& obj){
            indexStr=obj.indexStr;
            name=obj.name;
            type=obj.type;
            createTime=obj.createTime;
            dbName=obj.dbName;
            param=obj.param;
        }
        tagMethod& operator=(const tagMethod& obj){
            Copy(obj);
            return *this;
        }
        bool operator!=(const tagMethod& obj)const
        {
            return (!(*this==obj));
        }
        tagMethod* Clone() const
        {
            return (new tagMethod(*this));
        }
    } Method,*PMethod;
    typedef struct tagStepOfFlow{
        int index;
        std::string name;
        std::string param;
        std::string nameOfSameStep;
        int indexOfSameStep;
        tagStepOfFlow(){
            Clear();
        }
        void Clear(){
            index=-1;
            name="";
            param="";
            nameOfSameStep="";
            indexOfSameStep=-1;
        }
        bool operator==(const tagStepOfFlow& obj) const{
            bool ret=false;
            if (index==obj.index &&
                name==obj.name &&
                param==obj.param &&
                nameOfSameStep==obj.nameOfSameStep &&
                indexOfSameStep==obj.indexOfSameStep)
            {
                ret=true;
            }
            return ret;
        }
        tagStepOfFlow(const tagStepOfFlow& obj){
            Copy(obj);
        }
        void Copy(const tagStepOfFlow& obj){
            index=obj.index;
            name=obj.name;
            param=obj.param;
            nameOfSameStep=obj.nameOfSameStep;
            indexOfSameStep=obj.indexOfSameStep;
        }
        tagStepOfFlow& operator=(const tagStepOfFlow& obj){
            Copy(obj);
            return *this;
        }
        bool operator!=(const tagStepOfFlow& obj)const
        {
            return (!(*this==obj));
        }
        tagStepOfFlow* Clone() const
        {
            return (new tagStepOfFlow(*this));
        }
    } StepOfFlow, *PStepOfFlow;

    typedef struct tagFlowOfTask{
        std::string indexStr;
        std::string name;
        std::string type;
        std::string createTime;
        std::string dbName;
        std::vector<StepOfFlow> steps;
        tagFlowOfTask(){
            Clear();
        }
        void Clear(){
            indexStr="";
            name="";
            type="";
            createTime="";
            dbName="";
            steps.clear();
        }
        bool operator==(const tagFlowOfTask& obj) const{
            bool ret=false;
            if (indexStr==obj.indexStr &&
                name==obj.name &&
                type==obj.type &&
                createTime==obj.createTime &&
                dbName==obj.dbName && 
                steps==obj.steps)
            {
                ret=true;
            }
            return ret;
        }
        tagFlowOfTask(const tagFlowOfTask& obj){
            Copy(obj);
        }
        void Copy(const tagFlowOfTask& obj){
            indexStr=obj.indexStr;
            name=obj.name;
            type=obj.type;
            createTime=obj.createTime;
            dbName=obj.dbName;
            steps.clear();
            for (int i=0;i<int(obj.steps.size());i++)
            {
                steps.push_back(obj.steps[i]);
            }
        }
        tagFlowOfTask& operator=(const tagFlowOfTask& obj){
            Copy(obj);
            return *this;
        }
        bool operator!=(const tagFlowOfTask& obj)const
        {
            return (!(*this==obj));
        }
        tagFlowOfTask* Clone() const
        {
            return (new tagFlowOfTask(*this));
        }
    } FlowOfTask,*PFlowOfTask;

    typedef struct tagTask{
        std::string sampleName;
        std::string testFlow;
        std::string testMethod;
        FlowOfTask flowoftask;
        Method method;
        std::string runStatus;
        int testChannel;
        double content; // hanliang
        tagTask(){
            Clear();
        }
        tagTask(const tagTask& obj){
            Copy(obj);
        }

        #ifdef ENABLE_CAS6
        int circleNo;
        std::string batchNo;
        std::string serailNo;
        std::string detectStatus;
        std::string blank;
        int sampleDetectStrategy;
        void Clear(){
            blank="";
            circleNo=1;
            batchNo="";
            sampleName="";
            serailNo="";
            detectStatus="";
            content=0.0;
            testFlow="";
            testMethod="";
            flowoftask.Clear();
            method.Clear();
            runStatus="";
            testChannel=-1;
            sampleDetectStrategy=-1;
        }
        
        void Copy(const tagTask& obj){
            blank=obj.blank;
            circleNo=obj.circleNo;
            batchNo=obj.batchNo;
            sampleName=obj.sampleName;
            serailNo=obj.serailNo;
            detectStatus=obj.detectStatus;
            content=obj.content;
            testFlow=obj.testFlow;
            testMethod=obj.testMethod;
            flowoftask=obj.flowoftask;
            method=obj.method;
            runStatus=obj.runStatus;
            testChannel=obj.testChannel;
            sampleDetectStrategy=obj.sampleDetectStrategy;
        }
        
        bool operator==(const tagTask& obj) const{
            bool ret=false;
            if (batchNo==obj.batchNo &&
                blank==obj.blank &&
                circleNo==obj.circleNo &&
                sampleName==obj.sampleName &&
                serailNo==obj.serailNo &&
                detectStatus==obj.detectStatus &&
                content==obj.content &&
                testFlow==obj.testFlow && 
                testMethod==obj.testMethod && 
                flowoftask==obj.flowoftask && 
                method==obj.method &&
                testChannel==obj.testChannel &&
                sampleDetectStrategy==obj.sampleDetectStrategy &&
                runStatus==obj.runStatus)
            {
                ret=true;
            }
            return ret;
        }
        #else
        std::string sampleInput;
        std::string getSampleMethod;
        std::string targetElement;
        std::string unit; 
        std::string standard1Condition;
        std::string standard2Condition;
        double standard1;
        double standard2;
        std::string getSamplePump;
        int workingMode;
        int interval;
        
        void Clear(){
            sampleInput="";
            sampleName="";
            getSampleMethod="";
            targetElement="";
            content=0.0;
            unit="";
            standard1Condition="";
            standard2Condition="";
            standard1=0.0;
            standard2=0.0;
            testChannel=-1;
            getSamplePump="";
            testFlow="";
            testMethod="";
            workingMode=-1;
            interval=-1;
            flowoftask.Clear();
            method.Clear();
            runStatus="";
        }
        
        void Copy(const tagTask& obj){
            sampleInput=obj.sampleInput;
            sampleName=obj.sampleName;
            getSampleMethod=obj.getSampleMethod;
            targetElement=obj.targetElement;
            content=obj.content;
            unit=obj.unit;
            standard1Condition=obj.standard1Condition;
            standard2Condition=obj.standard2Condition;
            standard1=obj.standard1;
            standard2=obj.standard2;
            testChannel=obj.testChannel;
            getSamplePump=obj.getSamplePump;
            testFlow=obj.testFlow;
            testMethod=obj.testMethod;
            workingMode=obj.workingMode;
            interval=obj.interval;
            flowoftask=obj.flowoftask;
            method=obj.method;
            runStatus=obj.runStatus;
        }
        bool operator==(const tagTask& obj) const{
            bool ret=false;
            if (sampleInput==obj.sampleInput &&
                sampleName==obj.sampleName &&
                getSampleMethod==obj.getSampleMethod &&
                targetElement==obj.targetElement &&
                content==obj.content &&
                getSamplePump==obj.getSamplePump &&
                unit==obj.unit && 
                standard1Condition==obj.standard1Condition && 
                standard2Condition==obj.standard2Condition && 
                standard1==obj.standard1 && 
                standard2==obj.standard2 && 
                testChannel==obj.testChannel && 
                testFlow==obj.testFlow && 
                testMethod==obj.testMethod && 
                workingMode==obj.workingMode && 
                interval==obj.interval && 
                flowoftask==obj.flowoftask && 
                method==obj.method &&
                runStatus==obj.runStatus)
            {
                ret=true;
            }
            return ret;
        }
        #endif
        tagTask& operator=(const tagTask& obj){
            Copy(obj);
            return *this;
        }
        bool operator!=(const tagTask& obj)const
        {
            return (!(*this==obj));
        }
        tagTask* Clone() const
        {
            return (new tagTask(*this));
        }
    } Task,*PTask;

    #define SYSTEMINFODBNAME "NoLoginInfo"
    #define AUDITTRAILLOGDBNAME "AuditTrailLog"
    #define USERINFODBNAME "UserInfo"
    #define USERGROUPDBNAME "UserGroupInfo"
    #define SERIALPORTDBNAME "SerialPortInfo"
    #define AUDITTRAILDBRECORD "AUDITTRAILDBRECORD"
    #define DATACHARTMANAGENAME "DataChartManageInfo"
    #define METHODMANAGENAME "MethodInfo"
    #define FLOWMANAGEDBNAME "FlowInfo"
    #define TaskSequenceDBName "TaskSequence"
    #define TASKRUNRECORDINFODBNAME "TaskRunRecordInfo"
    #define TASKRUNSTATUSNAME "TaskRunStatusInfo"
    #define FlowDBName "Flow"
    #define MethodDBName "Method"
    #define CHANNELMANAGENAME "ChannelInfo"
    #define ChannelDBName "Channel"
    #define ReagentLinkDeviceDBName "ReagentDevice"
    #define ReagentDBName "Reagent"
    #define ScannerDBName "ScannerInfo"


    #define DB_PATH "/database/HG.db"
    #define DATA_DB_PATH "/database/HGData.db"
    #define LOG_DB_PATH "/database/HGLog.db"

    class RWDb{
        public:
            RWDb();
            ~RWDb();

            static void openDB(const std::string &basePath);
            static void closeDB();
            static void clearDB(const std::string& tableName);

            //------------------------base -----------------------------//
            static std::string readCurDirPath();
            static void copyTable(const std::string& sourceDBName,
                const std::string &targetDbName,const std::string &tableName);

            //------------------------read & write---------------------------//
            static void recordSingleInfo(std::string dbName, const std::map<std::string, std::string> &infoS);
            static std::vector<std::map<std::string,std::string>> readRecord(std::string dbName, std::map<std::string,std::string> &infoS);
            static void readSingleInfo(std::string dbName, std::map<std::string, std::string> &infoS);
            static void deleteRecord(std::string dbName, std::string deleteKey, std::string deleteValue);
            static void deleteDB(const std::string &dbName);
            static std::vector<std::string> getAllTables(std::string key);

            //------------------------login------------------------------//
            static std::string readLoginName();
            static bool isLoginRightTime();

            //------------------------user------------------------------//
            static std::string getUserInfoLastCount();
            static std::vector<std::map<std::string,std::string>> readUserInfo();
            static std::vector<std::map<std::string,std::string>> readUserGroupInfo();
            static void writeUserInfo(const std::map<std::string,std::string> &infoS);
            static void writeUserGroupInfo(const std::map<std::string,std::string> &infoS);
            static void deleteUserGroupInfo(const std::string &value);

            //-------------------------audit log -----------------------------//
            static std::vector<std::map<std::string,std::string>> readAuditTrailLog(const std::string &tableName="");
            static std::vector<std::string> getAllAuditLogTables();
            static void writeAuditTrailLog(const std::string &logContent);
            static int readAuditTrailLogCount(const std::string &tableName="");
            static void deleteAuditTrail();
            
            // 高性能日志查询接口（支持SQL条件过滤和分页）
            struct AuditLogQueryParam {
                std::string timeFrom;      // 开始时间（格式：YYYY-MM-DD HH:MM:SS）
                std::string timeTo;        // 结束时间
                std::string keyword;       // 关键词（匹配Time/Operator/LogContent）
                int pageSize;              // 每页条数
                int pageIndex;             // 页码（从0开始）
                AuditLogQueryParam() : pageSize(100), pageIndex(0) {}
            };
            
            // 单表条件查询（带分页）
            static std::vector<std::map<std::string,std::string>> readAuditTrailLogWithFilter(
                const std::string &tableName,
                const AuditLogQueryParam &param,
                int &outTotalCount);
            
            // 全库条件查询（带分页，自动跨表查询）
            static std::vector<std::map<std::string,std::string>> searchAuditTrailLogGlobal(
                const AuditLogQueryParam &param,
                int &outTotalCount);
            
            // 获取符合条件的总记录数（用于分页）
            static int countAuditTrailLogWithFilter(
                const std::string &tableName,
                const AuditLogQueryParam &param);
            
            // 创建日志表索引（优化查询性能）
            static void createAuditLogIndexes(const std::string &tableName);


            //-------------------------serial port------------------------------//
            static void writeSerialPortInfo(const std::vector<std::string> &serialPorts);
            static std::vector<std::string> readSerialPortInfo();
            static std::string getLastLoginTime();

            //-------------------------method ------------------------------//
            static Method readMethodParam(std::string dbName);
            static std::map<std::string,std::string> getMethodMap(int index,Method method);
            static std::vector<std::map<std::string,std::string>> getMethodParamMap(Method method);
            static std::vector<std::map<std::string,std::string>> readMethodInfo();
            static std::vector<std::string> getMethodNames();
            static void clearMethodManageRecord();
            static std::string decodeMethodDBName(const std::string &methodName);
            static std::string getMethodName(const std::string &flowName);
            static void writeMethodManageRecord(const std::map<std::string,std::string> &infoS);
            static void writeMethodRecord(std::string dbName,bool coverFlag,const std::vector<std::map<std::string,std::string>> &infoSS);

            //-------------------------flow-------------------------------------//
            static std::vector<std::map<std::string,std::string>> getFlowStepMap(FlowOfTask flow);
            static std::map<std::string,std::string> getFlowMap(int index,FlowOfTask flow);
            static std::vector<std::map<std::string, std::string>> readFlowInfo();
            static FlowOfTask readFlowOfTask(std::string dbName);
            static std::vector<std::string> getFlowNames();
            static void clearFlowManageRecord();
            static std::string decodeFlowDBName(const std::string &flowName);
            static void writeFlowManageRecord(const std::map<std::string,std::string> &infoS);
            static void writeFlowRecord(std::string dbName,bool coverFlag,const std::vector<std::map<std::string,std::string>> &infoSS);  

            //-------------------------task-------------------------------------//
            static std::map<std::string,std::string> getTaskMap(int index,Task taskInfo);
            static Task getTaskSFromMap(std::map<std::string, std::string> infoS);
            static std::vector<std::map<std::string, std::string>> readTaskInfo(const std::string& taskSeqName);
            static void writeTaskRecord(bool coverFlag,const std::string& tableName,const std::vector<std::map<std::string,std::string>> &infoSS); 
            static void insertTaskRunInfo(const std::string &startTime,const std::string &tableName);
            static std::string getTaskRunFlag();
            static std::vector<std::map<std::string,std::string>> getTaskRunInfo();
            static void setTaskRunRecordDataDB(const std::string &dbName);
            static std::string getTaskRunRecordDataDB();
            static void setTaskRunFlag(std::string);
            static void resetTaskRunFlag();
            static void resetTaskRunStatus(const std::string &tableName);

            //-------------------------channel --------------------------------//
            static std::vector<std::map<std::string,std::string>> readChannelInfo();
            static void writeChannelManageRecord(const std::map<std::string,std::string> &infoS);
            static std::map<std::string,std::string> getChannelMap(int index,Channel channel);
            static void clearChannelManageRecord();
            static void deleteAllChannelModuleDB();    

            //-------------------------reagent --------------------------------//
            static std::vector<REAGENT> readReagentInfo(const std::string &reagentTableName);
            static std::map<std::string,std::string> getMapFromReagent(const REAGENT &info);
            static REAGENT getFromReagentMap(std::map<std::string,std::string> regentInfo,std::vector<std::map<std::string,std::string>> deviceInfos);
            static const std::vector<std::string> getReagentNoEditName();
            static const std::vector<std::string> getReagentLinkDeviceNoEditName();   
            static void writeReagentRecord(bool coverFlag,const std::string& tableName,const std::vector<REAGENT> &infoSS);
            static std::vector<std::map<std::string,std::string>> readReagentDeviceInfo(std::string dbName);
            static std::map<std::string,std::string> getMapFromDevices(const ReagentLinkDevice &devices);
            
            //----------------------------module ---------------------------//
            static std::vector<ModuleOfChannel> readModulesParam(std::string dbName);
            static std::vector<std::map<std::string,std::string>> getModulesMap(Channel info);
            static void writeModulesRecord(std::string dbName,bool coverFlag,const std::vector<std::map<std::string,std::string>> &infoSS);
        
            //----------------------------scanner---------------------------//
            static void writeScannerInfo(const std::map<std::string,std::string> &info);
            static std::map<std::string,std::string> readScannerInfo();
            
            //----------------------------错误处理和超时控制---------------------------//
            // 设置查询超时时间（毫秒）
            static void setQueryTimeout(int timeoutMs);
            // 获取最后一次错误信息
            static std::string getLastError();
            // 清除错误信息
            static void clearLastError();
            
        protected:
            // static SendInfo response;
            static HGSaveDataToDB dbOpera;
            static HGSaveDataToDB dataOpera;
            static HGSaveDataToDB logOpera;
            
            // 错误信息
            static std::string m_lastError;
            static int m_queryTimeoutMs;
    };
}
#ifdef __cplusplus
}
#endif
#endif