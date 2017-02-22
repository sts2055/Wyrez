//
//  BackendController.cpp
//  Wyrez
//
//  Created by stefan on 7/3/13.
//
//

#include "BackendController.h"
#include <curl/curl.h>

static const std::string kHeaderXParseApplicationId("X-Parse-Application-Id: gBfOVaV4FITEJacQJuqJe2gBuenI2ZZ7i8UXMNWs");
static const std::string kHeaderXParseRestAPIKey("X-Parse-REST-API-Key: lZ3osgub3jP769pkn8YLTfq3q3XKx8LD8xLgAh89");
static const std::string kHeaderContentTypeJSON("Content-Type: application/json");

static inline std::string urlEncode(std::string string)
{
    char* curlEscapedString = curl_escape( string.c_str(), string.length() );
    std::string pURLencodedJSON = std::string(curlEscapedString);
    curl_free(curlEscapedString);
    
    return pURLencodedJSON;
}

static BackendController *s_pBackendController = NULL; // pointer to singleton

BackendController::BackendController()
{
    
}

BackendController::~BackendController()
{
    CCLOG("cocos2d: deallocing ~BackendController %p", this);
    CCHttpClient* httpClient = CCHttpClient::getInstance();
    httpClient->destroyInstance();
}

BackendController* BackendController::getInstance()
{
    if (s_pBackendController == NULL) {
        s_pBackendController = new BackendController();
    }
    
    return s_pBackendController;
}

void BackendController::onHttpRequestCompleted(CCHttpClient *sender, CCHttpResponse *response)
{
    if (!response)
    {
        return;
    }
    
    // You can get original request type from: response->request->reqType
    if (0 != strlen(response->getHttpRequest()->getTag()))
    {
        CCLog("%s completed", response->getHttpRequest()->getTag());
    }
    
    int statusCode = response->getResponseCode();
    char statusString[64] = {};
    sprintf(statusString, "HTTP Status Code: %d, tag = %s", statusCode, response->getHttpRequest()->getTag());
    //m_labelStatusCode->setString(statusString);
    CCLog("response code: %d", statusCode);
    
    if (statusCode >= 200 && statusCode < 400)
        response->setSucceed(true);
    
    if (!response->isSucceed())
    {
        CCLog("response failed");
        CCLog("error buffer: %s", response->getErrorBuffer());
        //return;
    }
    
    // dump data
    std::vector<char> *buffer = response->getResponseData();
    printf("Http Test, dump data: ");
    for (unsigned int i = 0; i < buffer->size(); i++)
    {
        printf("%c", (*buffer)[i]);
    }
    printf("\n");
}

void BackendController::signupUser(std::string name, std::string password, std::string email)
{
    CCHttpRequest* request = new CCHttpRequest();
    request->setUrl("https://api.parse.com/1/users");
    request->setRequestType(CCHttpRequest::kHttpPost);
    std::vector<std::string> headers;
    headers.push_back(kHeaderXParseApplicationId);
    headers.push_back(kHeaderXParseRestAPIKey);
    headers.push_back(kHeaderContentTypeJSON);
    request->setHeaders(headers);
    request->setResponseCallback(this, httpresponse_selector(BackendController::onHttpRequestCompleted));
    
    std::ostringstream stringStream;
    stringStream << "{"
    << "\"username\":\"" << name << "\","
    << "\"password\":\"" << password << "\","
    << "\"email\":\"" << email << "\""
    << "}";
    
    // write the post data
    const char* postData = stringStream.str().c_str();
    request->setRequestData(postData, strlen(postData));
    
    request->setTag("POST signupUser");
    CCHttpClient::getInstance()->send(request);
    request->release();
}

void BackendController::loginUser(std::string name, std::string password)
{
    CCHttpRequest* request = new CCHttpRequest();
    
    std::string nameStr = urlEncode(std::string("username=" + name));
    std::string passwordStr = urlEncode(std::string("password=" + password));
    std::ostringstream stringStream;
    stringStream << "https://api.parse.com/1/login?"
    << nameStr
    << "&"
    << passwordStr;
    
    request->setUrl(stringStream.str().c_str());
    request->setRequestType(CCHttpRequest::kHttpGet);
    std::vector<std::string> headers;
    headers.push_back(kHeaderXParseApplicationId);
    headers.push_back(kHeaderXParseRestAPIKey);
    request->setHeaders(headers);
    request->setResponseCallback(this, httpresponse_selector(BackendController::onHttpRequestCompleted));
    CCHttpClient::getInstance()->send(request);
    request->setTag("GET loginUser");
    request->release();
}

void BackendController::requestPasswordReset(std::string email)
{
    CCHttpRequest* request = new CCHttpRequest();
    request->setUrl("https://api.parse.com/1/functions/hello");
    request->setRequestType(CCHttpRequest::kHttpPost);
    std::vector<std::string> headers;
    headers.push_back(kHeaderXParseApplicationId);
    headers.push_back(kHeaderXParseRestAPIKey);
    headers.push_back(kHeaderContentTypeJSON);
    request->setHeaders(headers);
    request->setResponseCallback(this, httpresponse_selector(BackendController::onHttpRequestCompleted));
    
    std::string dataString = "{\"email\":\"";
    dataString += email;
    dataString +="\"}";
    
    // write the post data
    const char* postData = dataString.c_str();
    request->setRequestData(postData, strlen(postData));
    
    request->setTag("POST requestPasswordReset");
    CCHttpClient::getInstance()->send(request);
    request->release();
}
