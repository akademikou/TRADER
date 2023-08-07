#include <curl\curl.h>
#include <curl\easy.h>
//define CURL_STATICLIB
#pragma comment(lib, "wldap32.lib" )
#pragma comment(lib, "crypt32.lib" )
#pragma comment(lib, "Ws2_32.lib")
//#pragma comment(lib, "libcurl_imp.lib")
#include <stdio.h>


int main(void)
{
    CURL* curl;
    CURLcode res;

    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, "http://google.com");
        res = curl_easy_perform(curl);

        /* always cleanup */
        curl_easy_cleanup(curl);
    }
    return 0;
}