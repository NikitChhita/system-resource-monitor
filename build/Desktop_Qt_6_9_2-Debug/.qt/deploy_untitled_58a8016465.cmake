include("/home/nikit-chhita/real-time-system-monitor/build/Desktop_Qt_6_9_2-Debug/.qt/QtDeploySupport.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/untitled-plugins.cmake" OPTIONAL)
set(__QT_DEPLOY_I18N_CATALOGS "qtbase")

qt6_deploy_runtime_dependencies(
    EXECUTABLE "/home/nikit-chhita/real-time-system-monitor/build/Desktop_Qt_6_9_2-Debug/untitled"
    GENERATE_QT_CONF
)
