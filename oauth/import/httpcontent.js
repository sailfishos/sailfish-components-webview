//: The default message which is displayed in the browser application after the redirect is complete
//% "Please close this browser window."
var translatedMessage = qsTrId("sailfish_components_oauth-la-close_external_browser")
var data = "HTTP/1.1 200 OK\r\n"
         + "Content-Type: text/html\r\n"
         + "Connection: close\r\n\r\n"
         + "<!DOCTYPE html>\r\n"
         + "<html>\n"
         + "<head>\n"
         + "<style type=text/css>\n"
         + "h1 { text-align: center; font-size: 10.0vw; font-family: 'Arial', Helvetica, sans-serif }\n"
         + "</style>\n"
         + "<body>\n"
         + "<h1>\n"
         + translatedMessage + "\n"
         + "</h1>\n"
         + "</body>\n"
         + "</html>\r\n\r\n";
