//
//  BackendController.cpp
//  Wyrez
//
//  Created by stefan on 7/3/13.
//
//

#include "BackendController.h"
#include <curl/curl.h>

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

std::string* BackendController::urlEncodeJSON(std::string json)
{
    
    char* curlEscapedString = curl_escape( json.c_str(), json.length() );
    std::string* pURLencodedJSON = new std::string(curlEscapedString);
    curl_free(curlEscapedString);
    
    return pURLencodedJSON;
}

void BackendController::test()
{
    /*CCHttpRequest* request = new CCHttpRequest();
    request->setUrl("https://api.parse.com/1/functions/hello");
    request->setRequestType(CCHttpRequest::kHttpPost);
    std::vector<std::string> headers;
    headers.push_back("X-Parse-Application-Id: gBfOVaV4FITEJacQJuqJe2gBuenI2ZZ7i8UXMNWs");
    headers.push_back("X-Parse-REST-API-Key: lZ3osgub3jP769pkn8YLTfq3q3XKx8LD8xLgAh89");
    headers.push_back("Content-Type: application/json");
    request->setHeaders(headers);
    request->setResponseCallback(this, httpresponse_selector(BackendController::onHttpRequestCompleted));
    
    // write the post data
    const char* postData = "{}";
    request->setRequestData(postData, strlen(postData));
    
    request->setTag("POST test2");
    CCHttpClient::getInstance()->send(request);
    request->release();*/
    
    std::cout << "where={\"score\":{\"$gte\":1000,\"$lte\":3000}}" << "\n";
    std::string* urlencodedjson = this->urlEncodeJSON("where={\"score\":{\"$gte\":1000,\"$lte\":3000}}");
    std::cout << *urlencodedjson << "\n";
    delete urlencodedjson;
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
    
    if (!response->isSucceed())
    {
        CCLog("response failed");
        CCLog("error buffer: %s", response->getErrorBuffer());
        return;
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

void BackendController::signupUser(std::string name, std::string password)
{
    CCHttpRequest* request = new CCHttpRequest();
    request->setUrl("https://api.parse.com/1/users");
    request->setRequestType(CCHttpRequest::kHttpPost);
    std::vector<std::string> headers;
    headers.push_back("X-Parse-Application-Id: gBfOVaV4FITEJacQJuqJe2gBuenI2ZZ7i8UXMNWs");
    headers.push_back("X-Parse-REST-API-Key: lZ3osgub3jP769pkn8YLTfq3q3XKx8LD8xLgAh89");
    headers.push_back("Content-Type: application/json");
    request->setHeaders(headers);
    request->setResponseCallback(this, httpresponse_selector(BackendController::onHttpRequestCompleted));
    
    std::ostringstream stringStream;
    stringStream << "{\"";
    std::string copyOfStr = stringStream.str();
    
    // write the post data
    const char* postData = "{}";
    request->setRequestData(postData, strlen(postData));
    
    request->setTag("POST test2");
    CCHttpClient::getInstance()->send(request);
    request->release();
}
