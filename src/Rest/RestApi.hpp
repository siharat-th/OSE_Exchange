//============================================================================
// Name        	: RestApi.hpp
// Author      	: centos
// Version     	:
// Copyright   	: Copyright (C) 2021 Katana Financial
// Date			: Oct 9, 2021 1:38:41 PM
//============================================================================


#ifndef SRC_REST_API_HPP_
#define SRC_REST_API_HPP_

#pragma once
#include <string>
using namespace std;

#include <KT01/Core/Settings.hpp>

class RestApi {
    public:
        // Static members for base URL and endpoints.
        inline static std::string API_SERVER;
        inline static std::string QUOTES;
        inline static std::string OPTS;
        inline static std::string ORDS;
        inline static std::string SECDEFS;
        inline static std::string NOTIFIER;
    
        inline static std::string API_QUOTES_GET;
        inline static std::string SECDEF_GET;
        inline static std::string SECDEF_GETSYMB;
        inline static std::string NOTIFY_PERSIST;
        inline static std::string ORDS_GET_TEST;
        inline static std::string ORDS_CNT_TEST;
        inline static std::string ORDS_DEL_TEST;
        inline static std::string ORDS_GET;
        inline static std::string ORDS_CNT;
        inline static std::string ORDS_DEL;
    
        // Constructor: loads settings from a file and initializes endpoints.
        // Default parameter allows using a default settings file.
        explicit RestApi(const std::string& settingsPath = "Settings.txt") {
            Settings settings(settingsPath);
            std::string base = settings.getString("RestApi.Server");
    
            API_SERVER = base;
            if (API_SERVER.back() != '/')
                API_SERVER += '/';
    
            // Construct base paths
            QUOTES   = API_SERVER + "quotes/";
            OPTS     = API_SERVER + "options/";
            ORDS     = API_SERVER + "orders/";
            SECDEFS  = API_SERVER + "secdefs/";
            NOTIFIER = API_SERVER + "notifications/";
    
            // Construct API endpoints
            API_QUOTES_GET = QUOTES + "get";
    
            SECDEF_GET     = SECDEFS + "get";
            SECDEF_GETSYMB = SECDEFS + "getSymb";
    
            NOTIFY_PERSIST = NOTIFIER + "persistNotification";
    
            ORDS_GET_TEST  = ORDS + "ordersV2_get_TEST";
            ORDS_CNT_TEST  = ORDS + "ordersV2_count_TEST?org={0}";
            ORDS_DEL_TEST  = ORDS + "ordersV2_del_TEST";
    
            ORDS_GET       = ORDS + "ordersV3_get";
            ORDS_CNT       = ORDS + "ordersV3_count?org={0}";
            ORDS_DEL       = ORDS + "ordersV3_del";
        }
};

    

#endif /* SRC_REST_URL_CONSTS_HPP_ */
