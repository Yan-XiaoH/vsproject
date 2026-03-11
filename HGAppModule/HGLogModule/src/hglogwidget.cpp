#include "hglogwidget.h"
#include <QHeaderView>
#include <QApplication>
#include "common.h"
#include <fstream>
#include <algorithm>
#include <QMessageBox>
#include <QBrush>
#include <QColor>
#include <sstream>
#include <iomanip>
#include <QRegularExpression>
#include <QtConcurrent/QtConcurrent>
#include <QElapsedTimer>

// 自定义表格项实现
HighlightedTableItem::HighlightedTableItem(const QString& text, const QString& keyword) {
    setHighlightedText(text, keyword);
}

void HighlightedTableItem::setHighlightedText(const QString& text, const QString& keyword) {
    if (keyword.isEmpty() || !text.contains(keyword, Qt::CaseInsensitive)) {
        setText(text);
        return;
    }
    
    // 使用HTML格式化高亮关键词
    QString highlightedText = text;
    QString lowerText = text.toLower();
    QString lowerKeyword = keyword.toLower();
    
    int pos = 0;
    QString result;
    int lastEnd = 0;
    
    while ((pos = lowerText.indexOf(lowerKeyword, pos)) != -1) {
        // 添加关键词前的文本
        result += highlightedText.mid(lastEnd, pos - lastEnd);
        // 添加高亮的关键词
        result += QString("<span style='background-color: yellow; color: red; font-weight: bold;'>%1</span>")
                  .arg(highlightedText.mid(pos, keyword.length()));
        
        lastEnd = pos + keyword.length();
        pos += keyword.length();
    }
    // 添加剩余文本
    result += highlightedText.mid(lastEnd);
    
    setText(result);
}

HGLogWidget::HGLogWidget(std::string lang,QWidget *parent) : QWidget(parent),
m_lang(lang),
m_curDisplayIndex(-1),
m_searchTotalCount(0),
m_searchCurrentPage(0),
m_isSearchMode(false)
{
    RWDb::writeAuditTrailLog(loadTranslation(m_lang,"Enter")+loadTranslation(m_lang,"Log"));
    m_auditLogTableNames = RWDb::getAllAuditLogTables();
    m_searchCondition.Clear();

    m_layout=new QGridLayout();
    this->setLayout(m_layout);

    m_inputsearchConditionW=NULL;
    m_inputsearchConditionW=new HGInputSearchConditionWidget(HG_MAX_SEARCH_RANGE,m_lang);
    connect(m_inputsearchConditionW,SIGNAL(signalKeyWord(QString)),this,SLOT(slotKeyWord(QString)));
    connect(m_inputsearchConditionW,SIGNAL(signalTimeFrom(QString)),this,SLOT(slotTimeFrom(QString)));
    connect(m_inputsearchConditionW,SIGNAL(signalTimeTo(QString)),this,SLOT(slotTimeTo(QString)));
    connect(m_inputsearchConditionW,SIGNAL(signalSearch()),this,SLOT(slotSearch()));
    connect(m_inputsearchConditionW,SIGNAL(signalClearSearch()),this,SLOT(slotClearSearch()));

    m_manipulateGroup=new QGroupBox(QString::fromStdString(loadTranslation(m_lang,"manipulate")));
    m_manipulateGroup->setStyleSheet("QGroupBox { font-size: 12pt; font-weight:bold;}");
    m_manipulateLayout=new QGridLayout();

    m_pageLabel=new QLabel("第"+QString::number(m_curDisplayIndex)+"页");
    m_saveLabel=new HGQLabel(false,getPath("/resources/V1/@1xmb-save 1.png")); 
    m_nextLabel=new HGQLabel(false,getPath("/resources/V1/@1xze-arrow 1.png")); 
    m_preLabel=new HGQLabel(false,getPath("/resources/V1/@1xze-arrow-left 1.png")); 
    connect(m_saveLabel,SIGNAL(leftClicked()),this,SLOT(slotSaveSearchLog()));
    connect(m_nextLabel,SIGNAL(leftClicked()),this,SLOT(slotNext()));
    connect(m_preLabel,SIGNAL(leftClicked()),this,SLOT(slotPre()));

    m_tableW=new QTableWidget(0,3);
    QStringList headers={"时间","日志内容","操作员"};
    m_tableW->setHorizontalHeaderLabels(headers);
    m_tableW->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_tableW->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    m_tableW->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_tableW->resizeRowsToContents();
    m_tableW->setEditTriggers(QAbstractItemView::NoEditTriggers);
    
    m_logTypeLabel=new QLabel(QString::fromStdString(loadTranslation(m_lang,"LogType")));
    m_logTypeComboBox=new QComboBox();
    m_logTypeComboBox->addItems({QString::fromStdString(loadTranslation(m_lang,"AuditTrail")),
                                 QString::fromStdString(loadTranslation(m_lang,"RunLog"))});
    m_logTypeComboBox->setCurrentIndex(0);
    connect(m_logTypeComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(slotLogTypeChanged(int)));
    slotLogTypeChanged(0);

    m_manipulateLayout->addWidget(m_saveLabel,0,2);
    m_manipulateLayout->addWidget(m_preLabel,0,3);
    m_manipulateLayout->addWidget(m_nextLabel,0,4);
    m_manipulateLayout->addWidget(m_pageLabel,0,5);
    m_manipulateLayout->addWidget(m_tableW,1,0,1,10);
    m_manipulateGroup->setLayout(m_manipulateLayout);

    m_layout->addWidget(m_inputsearchConditionW,0,1,1,3);
    m_layout->addWidget(m_logTypeLabel,0,6,1,1);
    m_layout->addWidget(m_logTypeComboBox,0,7,1,1);
    m_layout->addWidget(m_manipulateGroup,1,1,1,15);
    fnReadDB("");
}

bool HGLogWidget::closeWindow()
{
    if (m_inputsearchConditionW){
        if (m_inputsearchConditionW->closeWindow()){
            SAFE_DELETE(m_inputsearchConditionW);
        }
    }
    return true;
}

HGLogWidget::~HGLogWidget()
{
    // 确保异步搜索已取消并清理资源
    if (m_searchWatcher && m_searchWatcher->isRunning()) {
        m_searchCancelled.store(true);
        m_searchWatcher->cancel();
        m_searchWatcher->waitForFinished();
    }
    cleanupSearchResources();
}

void HGLogWidget::slotNext(){
    if (m_logTypeComboBox->currentIndex() == 0) {
        // 审计日志模式
        if (m_isSearchMode) {
            // 搜索模式下的分页
            int totalPages = (m_searchTotalCount + SEARCH_PAGE_SIZE - 1) / SEARCH_PAGE_SIZE;
            if (m_searchCurrentPage < totalPages - 1) {
                m_searchCurrentPage++;
                performGlobalSearch();
            } else {
                QMessageBox::warning(this, QString::fromStdString(HG_DEVICE_NAME),
                                 "已经是最后一页");
            }
        } else {
            // 普通模式下的分页
            if (m_curDisplayIndex < 0) return;
            if (m_curDisplayIndex < int(m_auditLogTableNames.size())-1) m_curDisplayIndex++;
            else {
                QMessageBox::warning(this, QString::fromStdString(HG_DEVICE_NAME),
                                 "已经是最后一页");
                m_curDisplayIndex=m_auditLogTableNames.size()-1;
            }
            std::string dbName=m_auditLogTableNames[m_curDisplayIndex];
            fnReadDB(dbName);
        }
    } else {
        // 运行日志模式
        QMessageBox::warning(this, QString::fromStdString(HG_DEVICE_NAME),
                         "已经是最后一页");
    }
}

void HGLogWidget::slotPre(){
    if (m_logTypeComboBox->currentIndex() == 0) {
        // 审计日志模式
        if (m_isSearchMode) {
            // 搜索模式下的分页
            if (m_searchCurrentPage > 0) {
                m_searchCurrentPage--;
                performGlobalSearch();
            } else {
                QMessageBox::warning(this, QString::fromStdString(HG_DEVICE_NAME),
                                 "已经是第一页");
            }
        } else {
            // 普通模式下的分页
            if (m_curDisplayIndex < 0) {
                QMessageBox::warning(this, QString::fromStdString(HG_DEVICE_NAME),
                                 "已经是第一页");
                m_curDisplayIndex=0;
            } else {
                m_curDisplayIndex--;
            }
            std::string dbName=m_auditLogTableNames[m_curDisplayIndex];
            fnReadDB(dbName);
        }
    } else {
        // 运行日志模式
        QMessageBox::warning(this, QString::fromStdString(HG_DEVICE_NAME),
                         "已经是第一页");
    }
}

int HGLogWidget::getTableNameIndex(const std::string &tableName){
    for (int i=0;i<int(m_auditLogTableNames.size());i++){
        if (m_auditLogTableNames[i] == tableName) {
            m_curDisplayIndex = i;
            break;
        }
    }
    if (tableName=="") m_curDisplayIndex=m_auditLogTableNames.size()-1;
    return m_curDisplayIndex;
}

void HGLogWidget::fnReadDB(const std::string &tableName){
    m_tableW->setRowCount(0);
    m_tableW->setUpdatesEnabled(false);
    
    switch (m_logTypeComboBox->currentIndex()){
        case 0:
        {
            int auditTrailLogCount=RWDb::readAuditTrailLogCount(tableName);
            if (auditTrailLogCount > 10000){
                QMessageBox::warning(this, QString::fromStdString(HG_DEVICE_NAME),
                                 QString::fromStdString(loadTranslation(m_lang,"TooManagLogFiles")));
                m_tableW->setUpdatesEnabled(true);
                return;
            }
            
            std::vector<std::map<std::string,std::string>> loginfos=RWDb::readAuditTrailLog(tableName);
            getTableNameIndex(tableName);
            m_pageLabel->setText("第"+QString::number(m_curDisplayIndex+1)+"页");
            
            m_tableW->setRowCount(loginfos.size());
            int traillogIndex = 0;
            
            for (int i =int(loginfos.size())-1;i>=0;i--){
                for (auto info:loginfos[i]){
                    int nameColIndex=m_logContentMap[info.first];
                    if (nameColIndex<0||nameColIndex>=m_tableW->columnCount())
                        continue;
                    m_tableW->setItem(traillogIndex,nameColIndex,new QTableWidgetItem(QString::fromStdString(info.second)));
                }
                traillogIndex++;
            }
            m_tableW->setRowCount(traillogIndex);
            break;
        }
        case 1:
        {
            std::vector<FileInfo> fileList;
            HGGetFilesNoBytes("/app/log/",".log",fileList);
            std::sort(fileList.begin(), fileList.end(), [](const FileInfo& a, const FileInfo& b) {
                return a.createtime < b.createtime;
            });
            
            for (int i = int(fileList.size()-1); i >= 0; i--)
            {
                std::ifstream file(fileList[i].filename);
                if (!file.is_open()) continue;

                std::string line;
                while (std::getline(file, line)){
                    m_tableW->insertRow(m_tableW->rowCount());
                    int pos=line.find_first_of(">");
                    m_tableW->setItem(m_tableW->rowCount()-1, 0, new QTableWidgetItem(QString::fromStdString(line.substr(0,pos-1))));
                    m_tableW->setItem(m_tableW->rowCount()-1, 1, new QTableWidgetItem(QString::fromStdString(line.substr(pos+1,line.length()-pos-1))));
                    m_tableW->setItem(m_tableW->rowCount()-1, 2, new QTableWidgetItem(QString::fromStdString(GlobalSingleton::instance().getSystemInfo("loginName"))));
                }
                file.close();
            }
            m_pageLabel->setText("第1页");
            break;
        }
        default:{
            break;
        }
    }
    m_tableW->setUpdatesEnabled(true);
}

void HGLogWidget::slotLogTypeChanged(int index){
    m_tableW->clear();
    m_tableW->setRowCount(0);
    m_logContentMap["Time"]=0;
    m_logContentMap["LogContent"]=1;
    m_logContentMap["Operator"]=2;
    
    m_isSearchMode = false;
    m_searchTotalCount = 0;
    m_searchCurrentPage = 0;
    m_currentKeyword.clear();
    
    switch (index){
        case 0:{
        QStringList headers={QString::fromStdString(loadTranslation(m_lang,"Time")),
                             QString::fromStdString(loadTranslation(m_lang,"LogContent")),
                             QString::fromStdString(loadTranslation(m_lang,"Operator"))};
        m_tableW->setColumnCount(headers.size());
        m_tableW->setHorizontalHeaderLabels(headers);
        m_tableW->horizontalHeaderItem(0)->setToolTip("Time");
        m_tableW->horizontalHeaderItem(1)->setToolTip("LogContent");
        m_tableW->horizontalHeaderItem(2)->setToolTip("Operator");
        break;
        }
        case 1:{
        QStringList headers1={QString::fromStdString(loadTranslation(m_lang,"Time")),
                                QString::fromStdString(loadTranslation(m_lang,"LogContent")),
                                QString::fromStdString(loadTranslation(m_lang,"Operator"))};
        m_tableW->setColumnCount(headers1.size());
        m_tableW->setHorizontalHeaderLabels(headers1);
        m_tableW->horizontalHeaderItem(0)->setToolTip("Time");
        m_tableW->horizontalHeaderItem(1)->setToolTip("LogContent");
        m_tableW->horizontalHeaderItem(2)->setToolTip("Operator");
        break;
        }
        default:
        break;
    }
    fnReadDB("");
}

void HGLogWidget::slotKeyWord(QString text){
    m_searchCondition.key=text.toStdString();
}

void HGLogWidget::slotTimeFrom(QString text){
    m_searchCondition.timeRangeFrom=text.toStdString();
    m_searchCondition.timeFrom=HGExactTime::currentTime();
    m_searchCondition.timeFrom.tm_year = atoi(m_searchCondition.timeRangeFrom.substr(0, 4).c_str());
    m_searchCondition.timeFrom.tm_mon = atoi(m_searchCondition.timeRangeFrom.substr(4, 2).c_str());
    m_searchCondition.timeFrom.tm_mday = atoi(m_searchCondition.timeRangeFrom.substr(6, 2).c_str());
    m_searchCondition.timeFrom.tm_hour = 0;
    m_searchCondition.timeFrom.tm_min = 0;
    m_searchCondition.timeFrom.tm_sec = 0;
}

void HGLogWidget::slotTimeTo(QString text){
    m_searchCondition.timeRangeTo=text.toStdString();
    m_searchCondition.timeTo=HGExactTime::currentTime();
    m_searchCondition.timeTo.tm_year = atoi(m_searchCondition.timeRangeTo.substr(0, 4).c_str());
    m_searchCondition.timeTo.tm_mon = atoi(m_searchCondition.timeRangeTo.substr(4, 2).c_str());
    m_searchCondition.timeTo.tm_mday = atoi(m_searchCondition.timeRangeTo.substr(6, 2).c_str());
    m_searchCondition.timeTo.tm_hour = 23;
    m_searchCondition.timeTo.tm_min = 59;
    m_searchCondition.timeTo.tm_sec = 59;
}

std::string HGLogWidget::formatTimeForSql(const std::string& timeStr) {
    // 将 YYYYMMDD 格式转换为 YYYY-MM-DD 00:00:00 格式
    if (timeStr.length() != 8) return timeStr;
    
    std::string year = timeStr.substr(0, 4);
    std::string month = timeStr.substr(4, 2);
    std::string day = timeStr.substr(6, 2);
    
    return year + "-" + month + "-" + day + " 00:00:00";
}

RWDb::AuditLogQueryParam HGLogWidget::buildSearchParam() {
    RWDb::AuditLogQueryParam param;
    param.pageSize = SEARCH_PAGE_SIZE;
    param.pageIndex = m_searchCurrentPage;
    param.keyword = m_searchCondition.key;
    
    if (!m_searchCondition.timeRangeFrom.empty()) {
        param.timeFrom = formatTimeForSql(m_searchCondition.timeRangeFrom);
    }
    if (!m_searchCondition.timeRangeTo.empty()) {
        // 结束时间设置为当天的23:59:59
        std::string year = m_searchCondition.timeRangeTo.substr(0, 4);
        std::string month = m_searchCondition.timeRangeTo.substr(4, 2);
        std::string day = m_searchCondition.timeRangeTo.substr(6, 2);
        param.timeTo = year + "-" + month + "-" + day + " 23:59:59";
    }
    
    return param;
}

void HGLogWidget::slotSearch(){
    // 如果已有搜索在进行中，先取消
    if (m_searchWatcher && m_searchWatcher->isRunning()) {
        m_searchCancelled.store(true);
        m_searchWatcher->cancel();
        m_searchWatcher->waitForFinished();
    }
    
    cleanupSearchResources();
    
    if (m_logTypeComboBox->currentIndex() == 0) {
        // 审计日志模式 - 执行异步全库搜索
        m_isSearchMode = true;
        m_searchCurrentPage = 0;
        m_currentKeyword = QString::fromStdString(m_searchCondition.key);
        performGlobalSearchAsync();
    } else {
        // 运行日志模式 - 执行异步搜索
        m_tableW->setRowCount(0);
        performRunLogSearchAsync();
    }
}

void HGLogWidget::performGlobalSearch(){
    // 同步搜索（保留用于兼容）
    // 显示搜索中提示
    m_pageLabel->setText("搜索中...");
    QApplication::processEvents();
    
    // 构建查询参数
    RWDb::AuditLogQueryParam param = buildSearchParam();
    
    // 使用新的高性能接口执行搜索
    m_currentPageResults = RWDb::searchAuditTrailLogGlobal(param, m_searchTotalCount);
    
    // 检查错误
    std::string error = RWDb::getLastError();
    if (!error.empty()) {
        showSearchError(QString::fromStdString(error));
        return;
    }
    
    if (m_currentPageResults.empty()) {
        m_tableW->setRowCount(0);
        m_pageLabel->setText("搜索完成，无结果");
        QMessageBox::information(this, QString::fromStdString(HG_DEVICE_NAME),
                             "未找到匹配的日志记录");
        return;
    }
    
    // 显示搜索结果
    displaySearchResults();
}

void HGLogWidget::performGlobalSearchAsync(){
    QMutexLocker locker(&m_searchMutex);
    
    // 重置取消标志
    m_searchCancelled.store(false);
    
    // 创建进度对话框
    m_progressDialog = new QProgressDialog(
        QString::fromStdString(loadTranslation(m_lang, "Searching")),
        QString::fromStdString(loadTranslation(m_lang, "Cancel")),
        0, 0, this);
    m_progressDialog->setWindowModality(Qt::WindowModal);
    m_progressDialog->setMinimumDuration(500); // 500ms后才显示
    m_progressDialog->setValue(0);
    
    // 连接取消按钮
    connect(m_progressDialog, &QProgressDialog::canceled, this, &HGLogWidget::onSearchCancelled);
    
    // 创建超时定时器
    m_searchTimeoutTimer = new QTimer(this);
    m_searchTimeoutTimer->setSingleShot(true);
    connect(m_searchTimeoutTimer, &QTimer::timeout, this, &HGLogWidget::onSearchTimeout);
    m_searchTimeoutTimer->start(SEARCH_TIMEOUT_MS);
    
    // 创建FutureWatcher
    m_searchWatcher = new QFutureWatcher<SearchResult>(this);
    connect(m_searchWatcher, &QFutureWatcher<SearchResult>::finished, 
            this, &HGLogWidget::onSearchFinished);
    
    // 显示搜索中提示
    m_pageLabel->setText("搜索中...");
    
    // 构建查询参数
    RWDb::AuditLogQueryParam param = buildSearchParam();
    
    // 启动异步搜索
    QFuture<SearchResult> future = QtConcurrent::run(
        doAsyncSearch, param, &m_searchCancelled);
    m_searchWatcher->setFuture(future);
}

SearchResult HGLogWidget::doAsyncSearch(const RWDb::AuditLogQueryParam& param, QAtomicBool* cancelFlag) {
    SearchResult result;
    QElapsedTimer timer;
    timer.start();
    
    try {
        // 检查是否已取消
        if (cancelFlag && cancelFlag->load()) {
            result.errorMessage = "搜索已取消";
            return result;
        }
        
        // 执行搜索
        int totalCount = 0;
        auto data = RWDb::searchAuditTrailLogGlobal(param, totalCount);
        
        // 再次检查是否已取消
        if (cancelFlag && cancelFlag->load()) {
            result.errorMessage = "搜索已取消";
            return result;
        }
        
        result.data = std::move(data);
        result.totalCount = totalCount;
        result.success = true;
        result.elapsedMs = timer.elapsed();
        
        // 检查数据库错误
        std::string dbError = RWDb::getLastError();
        if (!dbError.empty()) {
            result.errorMessage = QString::fromStdString(dbError);
            // 如果有部分结果，仍然标记为成功但带警告
            if (result.data.empty()) {
                result.success = false;
            }
        }
    } catch (const std::exception& e) {
        result.success = false;
        result.errorMessage = QString("搜索异常: %1").arg(e.what());
    } catch (...) {
        result.success = false;
        result.errorMessage = "搜索发生未知异常";
    }
    
    result.elapsedMs = timer.elapsed();
    return result;
}

void HGLogWidget::onSearchFinished(){
    QMutexLocker locker(&m_searchMutex);
    
    // 停止超时定时器
    if (m_searchTimeoutTimer) {
        m_searchTimeoutTimer->stop();
    }
    
    // 关闭进度对话框
    if (m_progressDialog) {
        m_progressDialog->close();
    }
    
    if (!m_searchWatcher) return;
    
    SearchResult result = m_searchWatcher->result();
    
    if (!result.success) {
        showSearchError(result.errorMessage.isEmpty() ? "搜索失败" : result.errorMessage);
        cleanupSearchResources();
        return;
    }
    
    // 保存结果
    m_currentPageResults = std::move(result.data);
    m_searchTotalCount = result.totalCount;
    
    if (m_currentPageResults.empty()) {
        m_tableW->setRowCount(0);
        m_pageLabel->setText("搜索完成，无结果");
        QMessageBox::information(this, QString::fromStdString(HG_DEVICE_NAME),
                             "未找到匹配的日志记录");
    } else {
        // 根据当前模式显示结果
        if (m_logTypeComboBox->currentIndex() == 0) {
            // 审计日志模式
            displaySearchResults();
        } else {
            // 运行日志模式
            displayRunLogSearchResults();
        }
        
        // 如果有警告信息，显示但不阻断
        if (!result.errorMessage.isEmpty()) {
            // 可以记录到日志，但不弹窗打扰用户
        }
    }
    
    cleanupSearchResources();
}

void HGLogWidget::displayRunLogSearchResults(){
    m_tableW->setUpdatesEnabled(false);
    m_tableW->clearContents();
    
    int rowCount = m_currentPageResults.size();
    m_tableW->setRowCount(rowCount);
    
    QString keyword = QString::fromStdString(m_searchCondition.key);
    
    for (int i = 0; i < rowCount; i++){
        const auto& row = m_currentPageResults[i];
        
        // 时间列（带高亮）
        QString timeStr = QString::fromStdString(row.at("Time"));
        HighlightedTableItem* timeItem = new HighlightedTableItem(timeStr, keyword);
        m_tableW->setItem(i, 0, timeItem);
        
        // 日志内容列（带高亮）
        QString contentStr = QString::fromStdString(row.at("LogContent"));
        HighlightedTableItem* contentItem = new HighlightedTableItem(contentStr, keyword);
        m_tableW->setItem(i, 1, contentItem);
        
        // 操作员列（带高亮）
        QString operatorStr = QString::fromStdString(row.at("Operator"));
        HighlightedTableItem* operatorItem = new HighlightedTableItem(operatorStr, keyword);
        m_tableW->setItem(i, 2, operatorItem);
    }
    
    m_tableW->setUpdatesEnabled(true);
    
    // 更新页码显示（运行日志不分页，显示总条数）
    m_pageLabel->setText(QString("共%1条").arg(m_searchTotalCount));
}

void HGLogWidget::onSearchCancelled(){
    m_searchCancelled.store(true);
    
    if (m_searchWatcher) {
        m_searchWatcher->cancel();
    }
    
    m_pageLabel->setText("搜索已取消");
    cleanupSearchResources();
}

void HGLogWidget::onSearchTimeout(){
    m_searchCancelled.store(true);
    
    if (m_searchWatcher) {
        m_searchWatcher->cancel();
    }
    
    showSearchError("搜索超时，请缩小搜索范围后重试");
    cleanupSearchResources();
}

void HGLogWidget::showSearchError(const QString& error){
    m_tableW->setRowCount(0);
    m_pageLabel->setText("搜索失败");
    QMessageBox::warning(this, QString::fromStdString(HG_DEVICE_NAME),
                         error);
}

void HGLogWidget::cleanupSearchResources(){
    if (m_searchTimeoutTimer) {
        m_searchTimeoutTimer->stop();
        delete m_searchTimeoutTimer;
        m_searchTimeoutTimer = nullptr;
    }
    
    if (m_progressDialog) {
        m_progressDialog->deleteLater();
        m_progressDialog = nullptr;
    }
    
    // 注意：m_searchWatcher在finished信号后自动处理
    if (m_searchWatcher && m_searchWatcher->isFinished()) {
        delete m_searchWatcher;
        m_searchWatcher = nullptr;
    }
}

void HGLogWidget::updateSearchProgress(){
    // 可以在这里更新进度条
    if (m_progressDialog) {
        m_progressDialog->setValue(m_progressDialog->value() + 1);
    }
}

void HGLogWidget::displaySearchResults(){
    m_tableW->setUpdatesEnabled(false);
    m_tableW->clearContents();
    
    int rowCount = m_currentPageResults.size();
    m_tableW->setRowCount(rowCount);
    
    for (int i = 0; i < rowCount; i++){
        const auto& row = m_currentPageResults[i];
        
        // 时间列（带高亮）
        QString timeStr = QString::fromStdString(row.at("Time"));
        HighlightedTableItem* timeItem = new HighlightedTableItem(timeStr, m_currentKeyword);
        m_tableW->setItem(i, 0, timeItem);
        
        // 日志内容列（带高亮）
        QString contentStr = QString::fromStdString(row.at("LogContent"));
        HighlightedTableItem* contentItem = new HighlightedTableItem(contentStr, m_currentKeyword);
        m_tableW->setItem(i, 1, contentItem);
        
        // 操作员列（带高亮）
        QString operatorStr = QString::fromStdString(row.at("Operator"));
        HighlightedTableItem* operatorItem = new HighlightedTableItem(operatorStr, m_currentKeyword);
        m_tableW->setItem(i, 2, operatorItem);
    }
    
    m_tableW->setUpdatesEnabled(true);
    
    // 更新页码显示
    int totalPages = (m_searchTotalCount + SEARCH_PAGE_SIZE - 1) / SEARCH_PAGE_SIZE;
    m_pageLabel->setText(QString("第%1/%2页 (共%3条)").arg(m_searchCurrentPage + 1).arg(totalPages).arg(m_searchTotalCount));
}

void HGLogWidget::performRunLogSearch(){
    // 同步版本（保留用于兼容）
    std::vector<FileInfo> fileList;
    HGGetFilesNoBytes("/app/log/",".log",fileList);
    std::sort(fileList.begin(), fileList.end(), [](const FileInfo& a, const FileInfo& b) {
        return a.createtime < b.createtime;
    });
    
    m_tableW->setUpdatesEnabled(false);
    
    for (int i = int(fileList.size()) - 1; i >= 0; i--){
        // 时间过滤
        if (!m_searchCondition.timeRangeFrom.empty() || !m_searchCondition.timeRangeTo.empty()){
            int timepos = fileList[i].filename.find_last_of("/");
            std::string filename = fileList[i].filename.substr(timepos + 1);
            timepos = filename.find_first_of("_");
            std::string timestr = filename.substr(0, timepos);
            
            HGExactTime fileTime = HGExactTime::currentTime();
            fileTime.tm_year = atoi(timestr.substr(0, 4).c_str());
            fileTime.tm_mon = atoi(timestr.substr(4, 2).c_str());
            fileTime.tm_mday = atoi(timestr.substr(6, 2).c_str());

            if (!m_searchCondition.timeRangeFrom.empty() && fileTime < m_searchCondition.timeFrom)
                continue;
            if (!m_searchCondition.timeRangeTo.empty() && fileTime > m_searchCondition.timeTo)
                continue;
        }
        
        std::ifstream file(fileList[i].filename);
        if (!file.is_open()) continue;

        std::string line;
        while (std::getline(file, line)){
            // 关键词过滤
            if (!m_searchCondition.key.empty()){
                if (line.find(m_searchCondition.key) == std::string::npos)
                    continue;
            }
            
            m_tableW->insertRow(m_tableW->rowCount());
            int pos=line.find_first_of(">");
            QString timeStr = QString::fromStdString(line.substr(0, pos > 0 ? pos-1 : 0));
            QString contentStr = QString::fromStdString(line.substr(pos > 0 ? pos+1 : 0));
            
            QString keyword = QString::fromStdString(m_searchCondition.key);
            m_tableW->setItem(m_tableW->rowCount()-1, 0, new HighlightedTableItem(timeStr, keyword));
            m_tableW->setItem(m_tableW->rowCount()-1, 1, new HighlightedTableItem(contentStr, keyword));
            m_tableW->setItem(m_tableW->rowCount()-1, 2, new HighlightedTableItem(
                QString::fromStdString(GlobalSingleton::instance().getSystemInfo("loginName")), keyword));
        }
        file.close();
    }
    
    m_tableW->setUpdatesEnabled(true);
    m_pageLabel->setText("第1页");
}

void HGLogWidget::performRunLogSearchAsync(){
    QMutexLocker locker(&m_searchMutex);
    
    // 重置取消标志
    m_searchCancelled.store(false);
    
    // 创建进度对话框
    m_progressDialog = new QProgressDialog(
        QString::fromStdString(loadTranslation(m_lang, "Searching")),
        QString::fromStdString(loadTranslation(m_lang, "Cancel")),
        0, 0, this);
    m_progressDialog->setWindowModality(Qt::WindowModal);
    m_progressDialog->setMinimumDuration(500);
    m_progressDialog->setValue(0);
    
    connect(m_progressDialog, &QProgressDialog::canceled, this, &HGLogWidget::onSearchCancelled);
    
    // 创建超时定时器
    m_searchTimeoutTimer = new QTimer(this);
    m_searchTimeoutTimer->setSingleShot(true);
    connect(m_searchTimeoutTimer, &QTimer::timeout, this, &HGLogWidget::onSearchTimeout);
    m_searchTimeoutTimer->start(SEARCH_TIMEOUT_MS);
    
    // 创建FutureWatcher
    m_searchWatcher = new QFutureWatcher<SearchResult>(this);
    connect(m_searchWatcher, &QFutureWatcher<SearchResult>::finished, 
            this, &HGLogWidget::onSearchFinished);
    
    // 显示搜索中提示
    m_pageLabel->setText("搜索中...");
    
    // 启动异步搜索
    QFuture<SearchResult> future = QtConcurrent::run(
        doAsyncRunLogSearch, m_searchCondition, &m_searchCancelled);
    m_searchWatcher->setFuture(future);
}

SearchResult HGLogWidget::doAsyncRunLogSearch(const SearchCondition& condition, QAtomicBool* cancelFlag) {
    SearchResult result;
    QElapsedTimer timer;
    timer.start();
    
    try {
        std::vector<FileInfo> fileList;
        HGGetFilesNoBytes("/app/log/",".log",fileList);
        std::sort(fileList.begin(), fileList.end(), [](const FileInfo& a, const FileInfo& b) {
            return a.createtime < b.createtime;
        });
        
        for (int i = int(fileList.size()) - 1; i >= 0; i--){
            // 检查是否已取消
            if (cancelFlag && cancelFlag->load()) {
                result.errorMessage = "搜索已取消";
                result.success = !result.data.empty(); // 如果有部分结果也算部分成功
                return result;
            }
            
            // 时间过滤
            if (!condition.timeRangeFrom.empty() || !condition.timeRangeTo.empty()){
                int timepos = fileList[i].filename.find_last_of("/");
                std::string filename = fileList[i].filename.substr(timepos + 1);
                timepos = filename.find_first_of("_");
                std::string timestr = filename.substr(0, timepos);
                
                HGExactTime fileTime = HGExactTime::currentTime();
                fileTime.tm_year = atoi(timestr.substr(0, 4).c_str());
                fileTime.tm_mon = atoi(timestr.substr(4, 2).c_str());
                fileTime.tm_mday = atoi(timestr.substr(6, 2).c_str());

                if (!condition.timeRangeFrom.empty() && fileTime < condition.timeFrom)
                    continue;
                if (!condition.timeRangeTo.empty() && fileTime > condition.timeTo)
                    continue;
            }
            
            std::ifstream file(fileList[i].filename);
            if (!file.is_open()) {
                if (!result.errorMessage.isEmpty()) result.errorMessage += "; ";
                result.errorMessage += QString("无法打开文件: %1").arg(QString::fromStdString(fileList[i].filename));
                continue;
            }

            std::string line;
            while (std::getline(file, line)){
                // 检查是否已取消
                if (cancelFlag && cancelFlag->load()) {
                    file.close();
                    result.errorMessage = "搜索已取消";
                    result.success = !result.data.empty();
                    return result;
                }
                
                // 关键词过滤
                if (!condition.key.empty()){
                    if (line.find(condition.key) == std::string::npos)
                        continue;
                }
                
                int pos=line.find_first_of(">");
                std::map<std::string,std::string> row;
                row["Time"] = line.substr(0, pos > 0 ? pos-1 : 0);
                row["LogContent"] = line.substr(pos > 0 ? pos+1 : 0);
                row["Operator"] = GlobalSingleton::instance().getSystemInfo("loginName");
                
                result.data.push_back(row);
            }
            file.close();
        }
        
        result.totalCount = result.data.size();
        result.success = true;
        
    } catch (const std::exception& e) {
        result.success = false;
        result.errorMessage = QString("搜索异常: %1").arg(e.what());
    } catch (...) {
        result.success = false;
        result.errorMessage = "搜索发生未知异常";
    }
    
    result.elapsedMs = timer.elapsed();
    return result;
}

void HGLogWidget::slotClearSearch(){ 
    m_searchCondition.Clear();
    m_isSearchMode = false;
    m_searchTotalCount = 0;
    m_searchCurrentPage = 0;
    m_currentKeyword.clear();
    m_currentPageResults.clear();
    fnReadDB("");
}

void HGLogWidget::slotSaveSearchLog(){
    if (m_tableW->rowCount()==0){
        QMessageBox::warning(this, QString::fromStdString(HG_DEVICE_NAME),
                             QString::fromStdString(loadTranslation(m_lang,"NoData")));
        return;
    }

    QDialog dialog(this);
    dialog.setWindowTitle(QString::fromStdString(loadTranslation(m_lang,"InputSaveName")));
    dialog.setWindowModality(Qt::ApplicationModal);
    QLabel* saveLabel=new QLabel(QString::fromStdString(loadTranslation(m_lang,"LogSaveType")));
    QComboBox* saveCombox=new QComboBox();
    saveCombox->addItems({"txt","csv","pdf"});
    enum {
        SAVE_TEXT,
        SAVE_CSV,
        SAVE_PDF
    };
    int logsavetype=SAVE_TEXT;
    connect(saveCombox,&QComboBox::currentTextChanged,[&](QString text){
        if (text=="txt"){
            logsavetype=SAVE_TEXT;
        }else if (text=="csv"){
            logsavetype=SAVE_CSV;
        } else{
            logsavetype=SAVE_PDF;
        }
    });
    QPushButton *okBtn=new QPushButton(QString::fromStdString(loadTranslation(m_lang,"Ok")));
    QPushButton *cancelBtn=new QPushButton(QString::fromStdString(loadTranslation(m_lang,"Cancel")));
    connect(okBtn,&QPushButton::clicked,[&]()
    {
        std::vector<std::map<std::string,std::string>> logList;
        
        // 如果是搜索模式，保存所有搜索结果；否则保存当前显示的内容
        if (m_isSearchMode && m_logTypeComboBox->currentIndex() == 0) {
            // 需要重新查询所有结果用于保存
            RWDb::AuditLogQueryParam param = buildSearchParam();
            param.pageSize = m_searchTotalCount;  // 获取所有结果
            param.pageIndex = 0;
            int totalCount;
            auto allResults = RWDb::searchAuditTrailLogGlobal(param, totalCount);
            
            for (const auto& row : allResults){
                std::map<std::string,std::string> log;
                log["Time"] = row.at("Time");
                log["LogContent"] = row.at("LogContent");
                log["Operator"] = row.at("Operator");
                logList.push_back(log);
            }
        } else {
            for (int i=0;i<m_tableW->rowCount();i++){
                std::map<std::string,std::string> log;
                for (int j=0;j<m_tableW->columnCount();j++){
                    std::string key=m_tableW->horizontalHeaderItem(j)->text().toStdString();
                    if (m_tableW->item(i,j)==nullptr) continue;
                    // 去除HTML标签
                    QString text = m_tableW->item(i,j)->text();
                    text.remove(QRegularExpression("<[^>]*>"));
                    log[key]=text.toStdString();
                }
                logList.push_back(log);
            }
        }
        
        std::string outlogPath=FileConfig::getDirPath()+"/outlog/";
        HGMkDir(outlogPath);
        HGExactTime curTime = HGExactTime::currentTime();
        std::string syncslice = curTime.toStringFromYearToSec();
        std::string logname=outlogPath+syncslice;
        switch (logsavetype){
            case SAVE_TEXT:{
                logname+=".txt";
                saveTableToTxt(logList,logname);
                break;
            }
            case SAVE_CSV:{
                logname+=".csv";
                saveTableToCsv(logList,logname);
                break;
            }
            case SAVE_PDF:{
                logname+=".pdf";
                saveTableToPdf(logList,logname,getPath("/resources/simhei.ttf"));
                break;
            }
            default:{
                break;
            }
        }
        dialog.close();
    });
    connect(cancelBtn,&QPushButton::clicked,[&]()
    {
        dialog.close();
    });
    QGridLayout *layout=new QGridLayout(&dialog);
    layout->addWidget(saveLabel,0,0);
    layout->addWidget(saveCombox,0,1);
    layout->addWidget(okBtn,1,0);
    layout->addWidget(cancelBtn,1,1);
    dialog.setLayout(layout);
    dialog.exec();
}
