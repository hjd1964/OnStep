// -----------------------------------------------------------------------------------
// Locale Chinese ISO ISO639-1 language code "cn"

// General
#define L_UPLOAD "上传"
#define L_DOWNLOAD "下载"
#define L_ACCEPT "确认"
#define L_CONTINUE "继续"
#define L_ARE_YOU_SURE "你确定吗"
#define L_LOGOUT "退出"

#define L_UT "格林威治时间"
#define L_LST "实时"
#define L_LONG "长度"
#define L_LAT "宽度"
#define L_EAST "东"
#define L_WEST "西"
#define L_ZENITH "天顶"
#define L_NCP "北天极"
#define L_SCP "南天极"

#define L_NONE "没有"
#define L_AUTO "自动"
#define L_OK "Ok"
#define L_ON "开"
#define L_OFF "关"
#define L_START "开始"
#define L_DELTA "对冲值"
#define L_ZERO "零"
#define L_SPAN "跨度"
#define L_POWER "功率"
#define L_GUIDE "引导"
#define L_CENTER "居中"
#define L_BLUE "蓝色"
#define L_GREEN "绿色"
#define L_RATE "速率"
#define L_RANGE "范围"
#define L_UNKNOWN "未知"
#define L_FAULT "故障"

// ---------------------------- Index ----------------------------
// date/time
#define L_TIME_BROWSER_UT "UT (网络浏览器)"

// weather
#define L_TELE_TEMPERATURE "望远镜温度"
#define L_TEMPERATURE "环境温度"
#define L_PRESSURE "气压"
#define L_HUMIDITY "相对湿度"
#define L_DEW_POINT "露点温度"

// operation
#define L_TRACKING "跟踪"
#define L_SITE "位置"
#define L_COORDINATES "坐标"
#define L_CURRENT "电流"
#define L_TARGET "目标"
#define L_ENCODER "编码器"
#define L_POLAR_ALIGN "极点对齐"

// pier side
#define L_PIER_SIDE "望远镜侧面"
#define L_MERIDIAN_FLIPS "子午线翻转"
#define L_ALIGN_MESSAGE "相对于安装量"
#define L_MERIDIAN_FLIP_W_TO_E "子午线翻转,从西到东"
#define L_MERIDIAN_FLIP_E_TO_W "子午线翻转,从东到西"

#define L_OPERATIONS "运转"

// parking
#define L_PARKING "停车"
#define L_PARKED "已停车"
#define L_NOT_PARKED "未停车"
#define L_PARKING "停车"
#define L_PARK_FAILED "停车失败"

// home
#define L_AT_HOME "在原位"
#define L_SLEWING "快速定位"
#define L_PPS_SYNC "同步卫星信号"

// tracking
#define L_REFR_COMP_RA "参考相对RA轴"
#define L_REFR_COMP_BOTH "两个轴的参考补偿"
#define L_FULL_COMP_RA "完整的RA轴"
#define L_FULL_COMP_BOTH "完全补偿两个轴"
#define L_TRACKING_RATE "跟踪速度"
#define L_CURRENT_MAXRATE "当前最大速率"
#define L_DEFAULT_MAXRATE "标准最大速率"
#define L_CURRENT_MAXSLEW "最大平移速度"

// driver status
#define L_DRIVER_STATUS "运转"
#define L_STATE "状态"
#define L_COMMS_FAILURE "命令失败"
#define L_STANDSTILL "停止"
#define L_OPEN_LOAD "打开负载"
#define L_SHORT_GND "接近地面"
#define L_SHUTDOWN_OVER "已关闭"
#define L_PRE_WARNING "预警"

// misc. errors/workload
#define L_INTERNAL_TEMP "内部控制器温度"
#define L_LAST_GENERAL_ERROR "最后一个一般(背景)错误"
#define L_WORKLOAD "工作量"
#define L_WIRELESS_SIGNAL_STRENGTH "无线 信号强度"

// --------------------------- Control ---------------------------

#if DISPLAY_SPECIAL_CHARS == ON
  #define HOME_CH "&#x1F3E0;"
  #define ARROW_DR "&#x27A5;"
  #define ARROW_UR "&#x27A6;"
  #define ARROW_R2 "&#x27A4;"
  #define CAUTION_CH "&#9888;"
  #define CLOCK_CH "&#x1F565;"
  #define ARROW_LL "&lt;&lt;"
  #define ARROW_L "&lt;"
  #define ARROW_R "&gt;"
  #define ARROW_RR "&gt;&gt;"
  #define ARROW_DD "&lt;&lt;"
  #define ARROW_D "&lt;"
  #define ARROW_U "&gt;"
  #define ARROW_UU "&gt;&gt;"
  #define SIDEREAL_CH "&#9733;"
  #define LUNAR_CH "&#9790;"
  #define SOLAR_CH "&#9737;"
#else
  #define HOME_CH "H"
  #define ARROW_DR "-&gt;"
  #define ARROW_UR "-&gt;"
  #define ARROW_R2 "&gt;"
  #define CAUTION_CH "!"
  #define CLOCK_CH "T"
  #define ARROW_LL "&lt;&lt;"
  #define ARROW_L "&lt;"
  #define ARROW_R "&gt;"
  #define ARROW_RR "&gt;&gt;"
  #define ARROW_DD "&lt;&lt;"
  #define ARROW_D "&lt;"
  #define ARROW_U "&gt;"
  #define ARROW_UU "&gt;&gt;"
  #define SIDEREAL_CH "*"
  #define LUNAR_CH "("
  #define SOLAR_CH "O"
#endif

#define BUTTON_N "N"
#define BUTTON_S "S"
#define BUTTON_E "E"
#define BUTTON_W "W"
#define BUTTON_SYNC "@"

#define L_CENTER_RATE "定心"
#define L_FIND_RATE "搜索"
#define L_FAST_RATE "快速"
#define L_VFAST_RATE "非常快"

#define L_HINT_CLOCK "设置日期/时间"
#define L_HINT_CONTINUE_GOTO "继续转到（如果在原位）"
#define L_HINT_FIND_HOME "搜索起始位置"
#define L_HINT_RESET_HOME "原始位置（重置）"
#define L_HINT_UNPARK "取消停车"
#define L_HINT_PARK "停车"
#define L_HINT_EMERGENCY_STOP "紧急停止!"
#define L_HINT_RATE_SIDEREAL "恒星速"
#define L_HINT_RATE_LUNAR "月球速"
#define L_HINT_RATE_SOLAR "太阳速"
#define L_HINT_FOC_SEL "选择聚焦器"
#define L_HINT_FOC_FIND_HOME "搜索焦点原位"
#define L_HINT_FOC_RESET_HOME "主页（重置）聚焦器"
#define L_HINT_ROT_FIND_HOME "搜索旋转轴首页"
#define L_HINT_ROT_RESET_HOME "主页（重置）聚焦器"
#define L_HINT_DEROTATE_ON "取消旋转"
#define L_HINT_ROTATOR_REV "反向旋转方向"
#define L_HINT_ROTATOR_PAR "将旋转器移至赤道角"
#define L_HINT_DEROTATE_OFF "关闭防旋转功能"

#define L_ALIGN "对齐"
#define L_AUX_FEATURES "辅助功能"

#define L_FOCUSER "聚焦器"
#define L_ROTATOR "旋转器"
#define L_CAMERA_EXPOSURE "接触"
#define L_CAMERA_DELAY "延迟"
#define L_CAMERA_COUNT "计数"

// ---------------------------- Library --------------------------

#define L_CATALOG "目录"
#define L_CATALOGS "目录"
#define L_RECS_AVAIL "可用的数据集"
#define L_NO_CATALOG "未选择目录"
#define L_CAT_DATA "数据（已下载或用于上传）"

#define L_CAT_EXAMPLE1 "字段：<br /> <pre>"
#define L_CAT_EXAMPLE2 "  第1行要求具有<br />的目录名称记录"
#define L_CAT_EXAMPLE3 "  由'$'后面最多10个字符组成<br /> <br />"
#define L_CAT_EXAMPLE4 "  对象名称是由11个字符组成的序列<br /> <br />"
#define L_CAT_EXAMPLE5 "  Cat是一个类别代码如下:<br />"
#define L_CAT_EXAMPLE6 "  赤经（Jnow）以小时,分钟和秒为单位<br />"
#define L_CAT_EXAMPLE7 "  赤纬（Jnow）以+/-度,分钟,秒为单位<br />"

#define L_CAT_NO_NAME "所选目录没有名称."
#define L_CAT_GET_NAME_FAIL "所选目录的名称选择失败."
#define L_CAT_NO_CAT "未选择目录."
#define L_SELECTED "已选择"
#define L_CAT_DOWNLOAD_FAIL "下载失败,数据无效."
#define L_CAT_DOWNLOAD_SUCCESS "下载成功."
#define L_CAT_DOWNLOAD_INDEX_FAIL "下载失败,无法索引目录."
#define L_CAT_DATA_REMOVED "目录数据已删除"
#define L_CAT_UPLOAD_FAIL "上传失败.无效的目录名称."
#define L_CAT_UPLOAD_INDEX_FAIL "索引目录名称记录时,上传失败."
#define L_CAT_DELETE_FAIL "删除目录名称记录时,上传失败."
#define L_CAT_WRITE_NAME_FAIL "写入目录名称记录时,上传失败."
#define L_CAT_UPLOAD_NO_NAME_FAIL "上传失败,第1行必须包含目录名称."
#define L_CAT_BAD_FORM "上传失败,行＃中的格式无效 "
#define L_CAT_UPLOAD_BAD_OBJECT_NAME "上传失败,＃行中的对象名称无效 "
#define L_CAT_BAD_CATEGORY "上传失败,行＃中的类别无效"
#define L_CAT_BAD_RA "上传失败,行＃中的RA格式无效 "
#define L_CAT_BAD_DEC "上传失败,行＃中的dec格式无效 "
#define L_CAT_UPLOAD_RA_FAIL "在行＃中发送RA时上传失败 "
#define L_CAT_UPLOAD_DEC_FAIL "在行＃中发送Dec时上传失败"
#define L_CAT_UPLOAD_LINE_FAIL "发送第＃行时上传失败"
#define L_CAT_UPLOAD_SUCCESS "上传成功"
#define L_CAT_UPLOAD_LINES_WRITTEN "写的行"
#define L_CAT_UPLOAD_SELECT_FAIL "上传失败,无法选择目录."
#define L_CAT_UPLOAD_NO_CAT "上传失败,未选择目录."
#define L_CAT_CLEAR "清除目录"
#define L_CAT_CLEAR_LIB "清除图书馆"

// --------------------------- Encoders --------------------------

// general settings
#define L_ENC_SYNC_TO_ENC "将OnStep同步到编码器"
#define L_ENC_SYNC_TO_ONS "将编码器同步到OnStep"
#define L_ENC_ZERO_TO_ONS "将绝对编码器零设置为OnStep"
#define L_ENC_AUTO_SYNC "将OnStep自动同步到编码器"
#define L_ENC_MAX_ANGLE "最大角度距离（编码器与OnStep相比）"
#define L_ENC_MAX_ANGLE_AXIS1 "（轴1，弧秒0到9999）"
#define L_ENC_MAX_ANGLE_AXIS2 "（轴2，弧秒0到9999）"
#define L_ENC_AUTO_RATE "步速控制"
#define L_ENC_SYNC_NOW "目前"

// encoder rate control
#define L_ENC_BINNED_SAMPLES "平均编码器值（合并）"
#define L_ENC_SAMPLES "平均编码器值（值）"
#define L_ENC_PROPORTIONAL "比例响应"
#define L_ENC_MIN_GUIDE "最小跟踪速度"
#define L_ENC_RATE_COMP "编码器速度补偿"
#define L_ENC_MAGN "范围"
#define L_ENC_PHASE "相位"
#define L_ENC_COUNTS "数字"
#define L_ENC_STAT_RATE_AXIS1 "轴1速率（恒星）"
#define L_ENC_STAT_INTPOL_COMP "补偿之间"
#define L_ENC_STAT_INTPOL_PHASE "初始阶段"

// ------------------------------ PEC ----------------------------

#define L_PEC_STATUS "定期错误校正 状态"
#define L_PEC_CONTROL "定期错误校正 控制"
#define L_PEC_PLAY "播放"
#define L_STOP "停止"
#define L_PEC_CLEAR "擦除"
#define L_PEC_REC "写入"
#define L_PEC_CLEAR_MESSAGE "擦除但擦除存储器，而不擦除EEPROM。在捕获期间，校正平均为3：1，除非擦除，否则首选缓冲区。在这种情况下使用了完整的更正."
#define L_PEC_EEWRITE "写入EEPROM"
#define L_PEC_EEWRITE_MESSAGE "将 定期错误校正 数据写入EEPROM，以便在控制器重新启动时可以使用。写入数据可能需要几秒钟."
#define L_PEC_NO_PEC_MESSAGE "已停用 定期错误校正 控制"
#define L_PEC_IDLE "闲"
#define L_PEC_WAIT_PLAY "正在等待开始播放"
#define L_PEC_PLAYING "播放"
#define L_PEC_WAIT_REC "记录正在等待索引信号"
#define L_PEC_RECORDING "绘图"
#define L_PEC_UNK "未知"
#define L_PEC_EEWRITING "写入EEPROM"

// --------------------------- Settings --------------------------

#define L_REFINE_POLAR_ALIGN "精确对齐极轴"
#define L_REFINE_PA "改变停车位"
#define L_REFINE_MESSAGE1 "赤道仪的设置和3+星的方向。在天球北极或南极附近引出一颗明亮的恒星，其偏角为50至80度（N或S）"
#define L_REFINE_MESSAGE2 "然后按[提炼 PA]键。使用方向键将星星重新定位在目镜中. "
#define L_REFINE_MESSAGE3 "（可选）再次重新调整赤道仪."

#define L_PARK "停车"
#define L_SET_PARK "设置停车位置"

#define L_TRK_FASTER "跟进"
#define L_TRK_SLOWER "更快"
#define L_TRK_RESET "重置(默认)"
#define L_TRK_COMP "跟踪速度的补偿（指向模型/折射）"
#define L_TRK_FULL "全速"
#define L_TRK_REFR "仅折射"
#define L_TRK_DUAL "两个轴"
#define L_TRK_SINGLE "一个轴"

#define L_BUZZER "转到警报，蜂鸣器"

#define L_MERIDIAN_FLIP_AUTO "越过边界时自动翻转子午线"
#define L_MERIDIAN_FLIP_NOW "目前"
#define L_MERIDIAN_FLIP_PAUSE "子午线翻转期间在原始位置暂停"

// ------------------------ Configuration ------------------------

#define L_BASIC_SET_TITLE "基本的:"

#define L_MAX_SLEW_SPEED "最大回转速度"

// goto Rate
#define L_VSLOW "非常慢 (0.5x)"
#define L_SLOW "慢 (0.75x)"
#define L_NORMAL "正常 (1x)"
#define L_FAST "快速 (1.5x)"
#define L_VFAST "更快 (2x)"

// backlash
#define L_BACKLASH_RANGE_AXIS1 "间隙，以弧秒为单位，0到3600"
#define L_BACKLASH_RANGE_AXIS2 "间隙，以弧秒为单位，0到3600"
#define L_BACKLASH_RANGE_AXIS345 "间隙，在步骤0到32767中"

// deadband/TCF
#define L_DEADBAND_RANGE_AXIS45 "TCF死区，在步骤1至32767中"
#define L_TCF_COEF_RANGE_AXIS45 "TCF系数，以微米/度为单位。 C -999.0至999.0"
#define L_TCF_COEF_EN_AXIS45 "TCF启用，0假或1真"

// limits
#define L_LIMITS_TITLE "范围和开销限制"
#define L_LIMITS_RANGE_HORIZON "地平线，以+/- 30度为单位"
#define L_LIMITS_RANGE_OVERHEAD "上限，以60到90度为单位"
#define L_LIMITS_RANGE_MERIDIAN_E "如果将望远镜放在坐骑的东侧，则指向子午线，以+/- 270度为单位"
#define L_LIMITS_RANGE_MERIDIAN_W "如果将望远镜放在坐骑西侧，则指向子午线，以+/- 270度为单位"

// location
#define L_LOCATION_TITLE "站点纬度，经度，UTC偏移"
#define L_LOCATION_LONG "经度，以度和分钟为单位。+/-180，W为+"
#define L_LOCATION_LAT "纬度，以度和分钟为单位。+/-90，N为+"
#define L_LOCATION_RANGE_UTC_OFFSET "UTC 偏移，以-14到+12小时为单位"
#define L_LOCATION_MESSAGE_UTC_OFFSET "标准时间的相对时区值（无冬季时间！）"

// advanced configuration
#define L_REVERT "还原为默认值"
#define L_ADV_SET_TITLE "高级"
#define L_ADV_SET_HEADER_MSG "重新启动 OnStep 后，以下更改将生效:"
#define L_ADV_MOUNT_TYPE "选择 1 GEM, 2 EQ Fork, 3 Alt/Azm"

// reset control
#define L_RESET_TITLE "OnStep 重置控制:"
#define L_RESET "重启 OnStep"
#define L_RESET_FWU "重启 OnStep 用于固件上传"
#define L_RESET_MSG1 "OnStep 必须处于“在家”或“停放”状态，否则重置请求将被忽略."

// ----------------------------  WiFi ----------------------------

#define L_WIFI_PERFORMANCE "性能和兼容性"
#define L_WIFI_CMD_TO "命令通道的串行读取超时:"
#define L_WIFI_WWW_TO "网络频道的串行读取超时："

#define L_WIFI_STA_TITLE "工作站模式（已连接到接入点）"
#define L_WIFI_PWD "密码"
#define L_WIFI_PWD_MSG "至少8个字符."
#define L_WIFI_EN_DHCP "开启DHCP"
#define L_WIFI_EN_DHCP_MSG "注意：如果开启了DHCP，以上地址将被忽略"
#define L_WIFI_EN_STA "开启工作站模式"
#define L_IP_ADDRESS "IP地址"
#define L_GATEWAY "网关"
#define L_SUBNET "子网掩码"
#define L_WIFI_CHA "频道"
#define L_WIFI_EN_AP_MODE "开启接入点模式"

#define L_WIFI_CONFIG_SECURITY "无线 配置安全性"

#define L_WIFI_AP "访问点模式"

#define L_WIFI_RESTART_MSG "您必须手动<u>重新启动</ u>才能使更改生效."

#define L_WIFI_TITLE "输入密码以更改 无线 配置:"

#define L_WIFI_ADVICE1 "设置：<br/> <br/>"
#define L_WIFI_ADVICE2 "仅激活两种模式<i>站点模式</ i> <b>或</ b> <i>接入点模式</ i>之一。同时激活这两个选项可能会导致性能问题！ <BR/>"
#define L_WIFI_ADVICE3 "如果控制器应连接到您的家庭WiFi，只需激活站点模式，然后 "
#define L_WIFI_ADVICE4 "停用接入点模式。如果控制器无法连接到您的家庭WLAN "
#define L_WIFI_ADVICE5 "接入点模式会自动重新激活。如果您在黑暗的地方和/或，这很有用 "
#define L_WIFI_ADVICE6 "保护自己不受锁定。<br/> <br/>"
#define L_WIFI_ADVICE7 "如果您不属于WLAN，则必须使用Arduino IDE重置ESP闪存（并因此返回默认设置）"
#define L_WIFI_ADVICE8 "在Arduino工具菜单中，将“擦除Flash'仅草图'”选项更改为“擦除Flash'所有Flash内容 "
#define L_WIFI_ADVICE9 "然后再次上传WiFi蓝牙固件."

// --------------------------- Constants -------------------------

#define L_DOWN_TITLE "未连接到OnStep的串行接口!"
#define L_DOWN_MESSAGE1 "如果波特率不符，请重新打开电源会有所帮助。<br /> <br />"
#define L_DOWN_MESSAGE2 "其他可能的原因：<br /> <ul>"
#define L_DOWN_MESSAGE3 "配置错误的OnStep波特率."
#define L_DOWN_MESSAGE4 "ESP波特率配置错误."
#define L_DOWN_MESSAGE5 "配置错误的MCU波特率."
#define L_DOWN_MESSAGE6 "COM端口的电缆连接不正确（如果交换了RX / TX，则会损坏硬件。）"
#define L_DOWN_MESSAGE7 "电源引脚的接线错误。OnStep的MCU与附加模块的MCU之间必须接地（GND）"

#define L_PAGE_STATUS "状态"
#define L_PAGE_CONTROL "控制"
#define L_PAGE_AUX "辅助的"
#define L_PAGE_LIBRARY "库"
#define L_PAGE_ENCODERS "编码器"
#define L_PAGE_PEC "定期校正"
#define L_PAGE_SETTINGS "设置"
#define L_PAGE_CONFIG "配置"
#define L_PAGE_WIFI "无线"

// ------------------------- MountStatus -------------------------

// general (background) errors
#define L_GE_NONE "没有"
#define L_GE_MOTOR_FAULT "电机/驱动器错误"
#define L_GE_ALT_MIN "地平线以下" 
#define L_GE_LIMIT_SENSE "限位开关已触发"
#define L_GE_DEC "超出了偏斜限制"
#define L_GE_AZM "超出了方位角限制"
#define L_GE_UNDER_POLE "超出了极点下限"
#define L_GE_MERIDIAN "超出了子午线限制"
#define L_GE_SYNC "超出同步安全性限制"
#define L_GE_PARK "停车失败"
#define L_GE_GOTO_SYNC "Goto 同步失败"
#define L_GE_UNSPECIFIED "未知错误"
#define L_GE_ALT_MAX "超出上限"
#define L_GE_WEATHER_INIT "天气传感器初始化失败"
#define L_GE_SITE_INIT "RTC初始化失败"
#define L_GE_NV_INIT "初始化 NV/EEPROM 损坏"
#define L_GE_OTHER "未知错误，代码"

// command errors
#define L_CE_NONE "没有错误"
#define L_CE_0 "回答 0"
#define L_CE_CMD_UNKNOWN "未知命令"
#define L_CE_REPLY_UNKNOWN "无效答案"
#define L_CE_PARAM_RANGE "参数超出范围"
#define L_CE_PARAM_FORM "无效的参数格式"
#define L_CE_ALIGN_FAIL "对齐失败"
#define L_CE_ALIGN_NOT_ACTIVE "对齐不活跃"
#define L_CE_NOT_PARKED_OR_AT_HOME "未停在原位"
#define L_CE_PARKED "已停放"
#define L_CE_PARK_FAILED "停车失败"
#define L_CE_NOT_PARKED "未停放"
#define L_CE_NO_PARK_POSITION_SET "未设置停车位置"
#define L_CE_GOTO_FAIL "goto 失败"
#define L_CE_LIBRARY_FULL "库已满"
#define L_CE_GOTO_ERR_BELOW_HORIZON "goto 到地平线下"
#define L_CE_GOTO_ERR_ABOVE_OVERHEAD "goto 超过天顶"
#define L_CE_SLEW_ERR_IN_STANDBY "在待机状态下平移"
#define L_CE_SLEW_ERR_IN_PARK "在停车位置旋转"
#define L_CE_GOTO_ERR_GOTO "已经转到"
#define L_CE_GOTO_ERR_OUTSIDE_LIMITS "已经转到极限范围外"
#define L_CE_SLEW_ERR_HARDWARE_FAULT "硬件故障"
#define L_CE_MOUNT_IN_MOTION "赤道仪启动中"
#define L_CE_GOTO_ERR_UNSPECIFIED "其他"
#define L_CE_UNK "未知的"
