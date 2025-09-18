include("/home/nikit-chhita/test/build/Desktop_Qt_6_9_2-Debug/.qt/QtDeploySupport.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/test-plugins.cmake" OPTIONAL)
set(__QT_DEPLOY_I18N_CATALOGS "qtbase")

qt6_deploy_runtime_dependencies(
    EXECUTABLE "/home/nikit-chhita/test/build/Desktop_Qt_6_9_2-Debug/test"
    GENERATE_QT_CONF
)
