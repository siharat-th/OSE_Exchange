# create required directories in the output directory
message(STATUS "Creating required directories in the output directory.")
file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/audit")
file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/MsgStorage")

# copy CME config files to output directory
message(STATUS "Copying config files to output directory.")
configure_file("cme_config_files/NRCert/Configuration/config.xml" "${CMAKE_BINARY_DIR}/cme_config_files/NRCert/Configuration/config.xml" COPYONLY)

# copy CME connection config files
configure_file("config/ALU-NR.conf" "${CMAKE_BINARY_DIR}/config/ALU-NR.conf" COPYONLY)

# copy CME instrument definition file
configure_file("config5/NRCert/insts.csv" "${CMAKE_BINARY_DIR}/config5/nr/cme.secdefs.nr.csv" COPYONLY)
configure_file("config5/NRCert/MSGW.csv" "${CMAKE_BINARY_DIR}/config5/nr/MSGW.csv" COPYONLY)
configure_file("config5/NRCert/prodgw.csv" "${CMAKE_BINARY_DIR}/config5/nr/prodgw.csv" COPYONLY)

# copy configuration files to output directory
configure_file("settings/CLN" "${CMAKE_BINARY_DIR}/CLN" FILE_PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ COPYONLY)
configure_file("settings/Menu.txt" "${CMAKE_BINARY_DIR}/Menu.txt" COPYONLY)
configure_file("settings/NEW_WEEK" "${CMAKE_BINARY_DIR}/NEW_WEEK" FILE_PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ COPYONLY)
configure_file("settings/SCRUNIT" "${CMAKE_BINARY_DIR}/SCRUNIT" FILE_PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ COPYONLY)
configure_file("settings/${AKL_TARGET_ENV}/BATMAN" "${CMAKE_BINARY_DIR}/BATMAN" FILE_PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ COPYONLY)
configure_file("settings/${AKL_TARGET_ENV}/cmeIL3.conf" "${CMAKE_BINARY_DIR}/cmeIL3.conf" COPYONLY)
configure_file("settings/${AKL_TARGET_ENV}/cmeSVC.conf" "${CMAKE_BINARY_DIR}/cmeSVC.conf" COPYONLY)
configure_file("settings/${AKL_TARGET_ENV}/ktn_sf.opf" "${CMAKE_BINARY_DIR}/ktn_sf.opf" COPYONLY)
configure_file("settings/${AKL_TARGET_ENV}/NetSettings.txt" "${CMAKE_BINARY_DIR}/NetSettings.txt" COPYONLY)
configure_file("settings/${AKL_TARGET_ENV}/SERVICE" "${CMAKE_BINARY_DIR}/SERVICE" FILE_PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ COPYONLY)
configure_file("settings/${AKL_TARGET_ENV}/Settings.txt" "${CMAKE_BINARY_DIR}/Settings.txt" COPYONLY)
configure_file("settings/${AKL_TARGET_ENV}/StubSettings.txt" "${CMAKE_BINARY_DIR}/StubSettings.txt" COPYONLY)
configure_file("settings/${AKL_TARGET_ENV}/SvcSettings.txt" "${CMAKE_BINARY_DIR}/SvcSettings.txt" COPYONLY)

# Check if simulator mode is used then override MSGW.csv file
if (${AKL_SIMULATOR_MODE})
  message(STATUS "AKL_SIMULATOR_MODE is set to ${AKL_SIMULATOR_MODE} so override MSGW.csv file.")  
  configure_file("settings/${AKL_TARGET_ENV}/simulator/MSGW.csv" "${CMAKE_BINARY_DIR}/config5/nr/MSGW.csv" COPYONLY)  
  configure_file("settings/${AKL_TARGET_ENV}/simulator/ALU-NR.conf" "${CMAKE_BINARY_DIR}/config/ALU-NR.conf" COPYONLY)  
endif()
