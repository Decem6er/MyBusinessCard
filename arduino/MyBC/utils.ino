//配置请求头
void configSendHeader() {
  if (isCache()) {
    webServer.sendHeader("Cache-Control", "max-age=691200, must-revalidate");
  } else {
    webServer.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  }
}

// 请求错误返回
void replyServerError(String msg) {
  webServer.send(500, FPSTR(TEXT_PLAIN), msg + "\r\n");
}

// 返回code
void replyServerCode(int code) {
  // 空内容会禁止Content-length标头，所以需要停止，因为我们没有发送内容长度
  webServer.send(code);
  webServer.client().stop();
}

// 获取首页地址
String getHomeUrl() {
  EEPROM.get(homeUrlStartAddr, homeUrl);
  if (homeUrl[0] == 0) {
    strcpy(homeUrl, "/index.html");
  }
  return homeUrl;
}

// 是否打开缓存
bool isCache() {
  uint8_t v = EEPROM.read(changeAddr);
  return v != 0;
}

// 是IP吗
bool isIp(String str) {
  for (size_t i = 0; i < str.length(); i++) {
    int c = str.charAt(i);
    if (c != '.' && (c < '0' || c > '9')) {
      return false;
    }
  }
  return true;
}

// IP转字符串
String ip2String(IPAddress ip) {
  String res = "";
  for (int i = 0; i < 3; i++) {
    res += String((ip >> (8 * i)) & 0xFF) + ".";
  }
  res += String(((ip >> 8 * 3)) & 0xFF);
  return res;
}

// 如果我们收到另一个域的请求，重定向到专属门户。在这种情况下返回true，这样页面处理程序就不会再次尝试处理请求。
bool captivePortal() {
  String hh = webServer.hostHeader();
  if (!isIp(hh) && hh != (String(myHostname) + ".local") && hh != (String(myHostname) + ".com") ) {
    Serial.println(webServer.uri() + "重定向到 captive portal");
    webServer.sendHeader("Location", String("http://") + ip2String(webServer.client().localIP()), true);
    replyServerCode(302);
    return true;
  }
  return false;
}

//获取EEPROM中的数据
void getEEPROM() {
  EEPROM.get(ssidAddr, ssid);
  EEPROM.get(pwdAddr, password);
  progress = EEPROM.read(progressAddr);
}
