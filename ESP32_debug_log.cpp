debug Log 
------------------------
#issue#1: 
	esp_core_dump_flash: No core dump partition found! 
	
#try:
	add core_dump part into partition.csv as following:
	# Name,   Type,  SubType, Offset,   Size,    Flags
	nvs,      data,  nvs,     0x9000,   0x5000,
	otadata,  data,  ota,     0xe000,   0x2000,
	app0,     app,   ota_0,   0x10000,  0x3d0000,
	fr,       data,        ,  0x3e0000, 0x20000,
	coredump,data,coredump,,64K,
	
#new issue(issue#3): keep rebooting
	
------------------------	
#issue#2: 
	There is a line seems not decoded well. 

	10:41:13.597 -> ets Jun  8 2016 00:22:57
	10:41:13.597 -> 
	10:41:13.597 -> rst:0x1 (POWERON_RESET),boot:0x13 (SPI_FAST_FLASH_BOOT)
	10:41:13.597 -> configsip: 0, SPIWP:0xee
	10:41:13.597 -> clk_drv:0x00,q_drv:0x00,d_drv:0x00,cs0_drv:0x00,hd_drv:0x00,wp_drv:0x00
	10:41:13.597 -> mode:DIO, clock div:1
	10:41:13.597 -> load:0x3fff0030,len:1344
	10:41:13.597 -> load:0x40078000,len:13924
->	10:41:13.597 -> ho 0 tail 12 room 4
|	10:41:13.597 -> load:0x40080400,len:3600
|	10:41:13.597 -> entry 0x400805f0
|	10:41:13.597 -> 
|	10:41:13.597 -> rst:0x3 (SW_RESET),boot:0x13 (SPI_FAST_FLASH_BOOT)
|	.......(keep rebooting)
|	
|----< this line is not decoded well?	
	
#try:
	Change flash frequency from default 80MHz to 40MHz.
	
	10:33:33.896 -> rst:0x3 (SW_RESET),boot:0x13 (SPI_FAST_FLASH_BOOT)
	10:33:33.896 -> configsip: 0, SPIWP:0xee
	10:33:33.896 -> clk_drv:0x00,q_drv:0x00,d_drv:0x00,cs0_drv:0x00,hd_drv:0x00,wp_drv:0x00
->	10:33:33.896 -> mode:DIO, clock div:2
|	10:33:33.929 -> load:0x3fff0030,len:1344
|	10:33:33.929 -> load:0x40078000,len:13896
|	10:33:33.929 -> load:0x40080400,len:3600
|	10:33:33.929 -> entry 0x400805f0
|
|---> The "clock div:2" means 80MHz is divided by 2, which is 40 MHz. 
------------------------
#issue#3:
	Keep rebooting.
	
#try: 
	Edit partition.csv as following:(No core dump column)
	# Name,   Type,  SubType, Offset,   Size,    Flags
	nvs,      data,  nvs,     0x9000,   0x5000,
	otadata,  data,  ota,     0xe000,   0x2000,
	app0,     app,   ota_0,   0x10000,  0x3d0000,
	fr,       data,        ,  0x3e0000, 0x20000,
	
	11:36:40.642 -> ets Jun  8 2016 00:22:57
	11:36:40.642 -> 
	11:36:40.642 -> rst:0x1 (POWERON_RESET),boot:0x3 (DOWNLOAD_BOOT(UART0/UART1/SDIO_REI_REO_V2))
	11:36:40.642 -> waiting for download
	11:36:47.379 -> ets Jun  8 2016 00:22:57
	11:36:47.379 -> 
	11:36:47.379 -> rst:0x1 (POWERON_RESET),boot:0x13 (SPI_FAST_FLASH_BOOT)
	11:36:47.379 -> configsip: 0, SPIWP:0xee
	11:36:47.379 -> clk_drv:0x00,q_drv:0x00,d_drv:0x00,cs0_drv:0x00,hd_drv:0x00,wp_drv:0x00
	11:36:47.379 -> mode:DIO, clock div:2
	11:36:47.379 -> load:0x3fff0030,len:1344
	11:36:47.379 -> load:0x40078000,len:13896
	11:36:47.379 -> load:0x40080400,len:3600
	11:36:47.379 -> entry 0x400805f0
	11:36:47.895 -> E (516) esp_core_dump_flash: ���ɕ�dump partition found!
	11:36:47.895 -> E (516) esp_core_dump_flash: No core dump partition found!
	
	Although core_dump error showed again, no more rebooting.
------------------------
#learning about Partition Table: 
	https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/partition-tables.html 
	https://blog.espressif.com/how-to-use-custom-partition-tables-on-esp32-69c0f3fa89c8
	https://robotzero.one/arduino-ide-partitions/
	
------------------------
#issue#4: 

	Let's forget about core_dump because that does not affect what we want.(It's onlt for those guy who wants to dump errors out, where the errors would be stored in core_dump region.)

	15:56:07.735 -> ets Jun  8 2016 00:22:57
	15:56:07.735 -> 
	15:56:07.735 -> rst:0xc (SW_CPU_RESET),boot:0x13 (SPI_FAST_FLASH_BOOT)
	15:56:07.735 -> configsip: 0, SPIWP:0xee
	15:56:07.735 -> clk_drv:0x00,q_drv:0x00,d_drv:0x00,cs0_drv:0x00,hd_drv:0x00,wp_drv:0x00
	15:56:07.780 -> mode:DIO, clock div:2
	15:56:07.780 -> load:0x3fff0030,len:1344
	15:56:07.780 -> load:0x40078000,len:13896
	15:56:07.780 -> load:0x40080400,len:3600
	15:56:07.780 -> entry 0x400805f0
	15:56:08.266 -> E (516) esp_core_dump_flash: ����core dump partition found!
	15:56:08.266 -> E (516) esp_core_dump_flash: No core dump partition found!
	15:56:08.738 -> Going to start WIFI.begin()...
	15:56:08.861 -> Already started WiFi.begin().
	15:56:09.862 -> Connecting to WiFi...
	15:56:09.862 -> E (1585) SPIFFS: spiffs partition could not be found
	15:56:09.862 -> An Error has occurred while mounting SPIFFS

#try(fail): 
	It seems if we partition the PSRAM(4MB for ESP32-CAM) with a partition.csv table, which exceeds 4MB, it will cause reboot loop. 
	I ask chatGPT about this phenomenon and it replied this is because ESP32-CAM cannot find the corresponding address to boot so that it keeps rebooting.
	
#try:
	partition.csv: 
	# Name,   Type,  SubType, Offset,   Size,    Flags
	nvs,      data,  nvs,     0x9000,   0x5000,
	otadata,  data,  ota,     0xe000,   0x2000,
	app0,     app,   ota_0,   0x10000,  0x200000,
	spiffs,   data,  spiffs,  0x210000, 0x1E0000,
	coredump, data,  coredump,0x3F0000, 0x10000,
	
	This partition.csv is justly 4MB size which fits to the ESP32-CAM PSRAM. And results in no errors!!!
	Not just solve the coredump error but also the spiffs error. 
	
	Here is the message from Serial Monitor: 
	15:47:58.773 -> ets Jun  8 2016 00:22:57
	15:47:58.773 -> 
	15:47:58.773 -> rst:0x1 (POWERON_RESET),boot:0x13 (SPI_FAST_FLASH_BOOT)
	15:47:58.817 -> configsip: 0, SPIWP:0xee
	15:47:58.817 -> clk_drv:0x00,q_drv:0x00,d_drv:0x00,cs0_drv:0x00,hd_drv:0x00,wp_drv:0x00
	15:47:58.817 -> mode:DIO, clock div:2
	15:47:58.817 -> load:0x3fff0030,len:1344
	15:47:58.817 -> load:0x40078000,len:13896
	15:47:58.817 -> load:0x40080400,len:3600
	15:47:58.817 -> entry 0x400805f0
	15:47:59.781 -> Total flash memory size: 4194304 bytes
->	15:47:59.781 -> Going to start WIFI.begin()...
->	15:47:59.891 -> Already started WiFi.begin().
|	15:48:00.915 -> Connecting to WiFi...
|	15:48:01.554 -> SPIFFS mounted successfully
|	15:48:01.554 -> IP Address: http://192.168.252.226
|
|---< This two lines are added by me to debug the previous issues.

------------------------
#issue#5: 
	
	Code like: 
	const char *ssid = "進擊的巨人"; 
	Serial.println("Connecting to WiFi '%s' ...\n", ssid);

	Get error: 
	error: no matching function for call to 'println(const char [28], const char*&)
	
#try: 
	Use printf() instead of println(). 
	Serial.printf("Connecting to WiFi '%s' ...\n", ssid);

------------------------
#issue#6: 
	
	14:46:24.629 -> Guru Meditation Error: Core  1 panic'ed (StoreProhibited). Exception was unhandled.
	14:46:24.629 -> 
	14:46:24.629 -> Core  1 register dump:
	14:46:24.629 -> PC      : 0x400d2ed2  PS      : 0x00060d30  A0      : 0x800d305a  A1      : 0x3ffb2220  
	14:46:24.629 -> A2      : 0x00000000  A3      : 0x0000000f  A4      : 0xd5410791  A5      : 0x3ffb21dc  
	14:46:24.629 -> A6      : 0x3ffc5ba8  A7      : 0x3ffb21dc  A8      : 0x00000000  A9      : 0x3ffb2200  
	14:46:24.662 -> A10     : 0x00000002  A11     : 0x3ffb2254  A12     : 0x00000003  A13     : 0x3ffbcc74  
	14:46:24.662 -> A14     : 0x3ffb21a0  A15     : 0x00000008  SAR     : 0x0000001f  EXCCAUSE: 0x0000001d  
	14:46:24.662 -> EXCVADDR: 0x00000048  LBEG    : 0x4008c85d  LEND    : 0x4008c86d  LCOUNT  : 0xffffffff  
	14:46:24.662 -> 
	14:46:24.662 -> 
	14:46:24.662 -> Backtrace: 0x400d2ecf:0x3ffb2220 0x400d3057:0x3ffb2240 0x400de53e:0x3ffb2290

#try: 
	

------------------------
ESP32-CAM hardware issue: 
	Antenna signal not good: Use hand, use anti-static bag...
	https://www.reddit.com/r/esp32/comments/c4iy1j/aithinker_esp32_cam_slow_video_stream_try/
	https://marksbench.com/electronics/esp32-cam-antenna-workaround/
	https://github.com/espressif/arduino-esp32/issues/4655
	https://github.com/espressif/arduino-esp32/issues/5834
	https://www.reddit.com/r/esp32/comments/r9g5jc/fixing_ymmv_the_poor_frame_rate_on_the_esp32cam/

------------------------
learning#7: 
	驗證儲存檔案成功的方式: 將https://randomnerdtutorials.com/esp32-cam-take-photo-display-web-server/ 的東西參考一下，他拍了照片並且可以顯示在網頁上，我們也可以用顯示在網頁上的方式來驗證是否有儲存照片成功。
------------------------	
learning#8: 
	用以下的程式碼去儲存照片:
	fb = esp_camera_fb_get();
    if (!fb) {
      Serial.println("Camera capture failed");
      return;
    }

    // Photo file name
    Serial.printf("Picture file name: %s\n", FILE_PHOTO);
    File file = SPIFFS.open(FILE_PHOTO, FILE_WRITE);

    // Insert the data in the photo file
    if (!file) {
      Serial.println("Failed to open file in writing mode");
    }
    else {
      file.write(fb->buf, fb->len); // payload (image), payload length
      Serial.print("The picture has been saved in ");
      Serial.print(FILE_PHOTO);
      Serial.print(" - Size: ");
      Serial.print(file.size());
      Serial.println(" bytes");
    }
    // Close the file
    file.close();
    esp_camera_fb_return(fb);
------------------------
learning#9: 
	html gzip檔案可以更改，但更改完再壓縮的時候，記得也要一併更新byte array的長度。
	
------------------------
warning#10:
	Use 40MHz and DIO to burn the ESP32-CAM. 
	And note that if you want to burn it, remove the power of ESP32-CAM and then put on the jumper.
	Don't put on the jumper while you're still using it.
	
------------------------
issue#11: 
	
	Cannot restart ble after stop it. 
	
code: (part of it)
	BLEDevice::init("ESP32_notify_server");
*	pServer = BLEDevice::createServer();		
	
debug: (part of it)
	11:22:48.175 -> ets Jun  8 2016 00:22:57
	11:22:48.175 -> 
	11:22:48.175 -> rst:0x1 (POWERON_RESET),boot:0x13 (SPI_FAST_FLASH_BOOT)
	11:22:48.175 -> configsip: 0, SPIWP:0xee
	11:22:48.222 -> clk_drv:0x00,q_drv:0x00,d_drv:0x00,cs0_drv:0x00,hd_drv:0x00,wp_drv:0x00
	11:22:48.222 -> mode:DIO, clock div:2
	11:22:48.222 -> load:0x3fff0030,len:1184
	11:22:48.222 -> load:0x40078000,len:13192
	11:22:48.222 -> load:0x40080400,len:3028
	11:22:48.222 -> entry 0x400805e4
	11:22:48.928 -> [    25][D][esp32-hal-cpu.c:244] setCpuFrequencyMhz(): PLL: 480 / 2 = 240 Mhz, APB: 80000000 Hz
	11:22:49.372 -> [   474][I][esp32-hal-psram.c:96] psramInit(): PSRAM enabled
	11:22:49.372 -> 

>1>	11:22:50.067 -> [  1203][V][BLEDevice.cpp:76] createServer(): >> createServer
/***correspoing code****/
	BLEServer* BLEDevice::createServer() {
>1>		ESP_LOGD(LOG_TAG, ">> createServer");
	#ifndef CONFIG_GATTS_ENABLE  // Check that BLE GATTS is enabled in make menuconfig
		ESP_LOGE(LOG_TAG, "BLE GATTS is not enabled - CONFIG_GATTS_ENABLE not defined");
		abort();
	#endif // CONFIG_GATTS_ENABLE
		m_pServer = new BLEServer();
		m_pServer->createApp(m_appId++);
		ESP_LOGD(LOG_TAG, "<< createServer");
		return m_pServer;
	} // createServer
	
	BLEServer::BLEServer() {
		m_appId            = ESP_GATT_IF_NONE;
		m_gatts_if         = ESP_GATT_IF_NONE;
		m_connectedCount   = 0;
		m_connId           = ESP_GATT_IF_NONE;
		m_pServerCallbacks = nullptr;
	} // BLEServer

>2>	11:22:50.100 -> [  1203][V][BLEServer.cpp:291] registerApp(): >> registerApp - 0
/***corresponding code****/
	void BLEServer::createApp(uint16_t appId) {
		m_appId = appId;
		registerApp(appId);
	} // createApp

	void BLEServer::registerApp(uint16_t m_appId) {
>2>		ESP_LOGD(LOG_TAG, ">> registerApp - %d", m_appId);
>3,4>	m_semaphoreRegisterAppEvt.take("registerApp"); // Take the mutex, will be released by ESP_GATTS_REG_EVT event.
		::esp_ble_gatts_app_register(m_appId);
>5,6>	m_semaphoreRegisterAppEvt.wait("registerApp");	// 這一行只是為了要確定semaphore有沒有在::esp_ble_gatts_app_register()被release，同時也確保esp_ble_gatts_app_register這個事件執行時不被打斷。
>B>		ESP_LOGD(LOG_TAG, "<< registerApp");
	} // registerApp


>3>	11:22:50.100 -> [  1204][V][FreeRTOS.cpp:189] take(): Semaphore taking: name: RegisterAppEvt (0x3ffd5fec), owner: <N/A> for registerApp
>4>	11:22:50.100 -> [  1214][V][FreeRTOS.cpp:198] take(): Semaphore taken:  name: RegisterAppEvt (0x3ffd5fec), owner: registerApp
/***corresponding code****/
	class BLEServer {
		public: ...
		private: 
			FreeRTOS::Semaphore m_semaphoreRegisterAppEvt 	= FreeRTOS::Semaphore("RegisterAppEvt");
			...
	}

	FreeRTOS::Semaphore::Semaphore(std::string name) {
		m_usePthreads = false;   	// Are we using pThreads or FreeRTOS?
		if (m_usePthreads) {
			pthread_mutex_init(&m_pthread_mutex, nullptr);
		} else {
			m_semaphore = xSemaphoreCreateMutex();
		}

		m_name      = name;
		m_owner     = std::string("<N/A>");
		m_value     = 0;
	}

	bool FreeRTOS::Semaphore::take(std::string owner) {
>3>		ESP_LOGD(LOG_TAG, "Semaphore taking: %s for %s", toString().c_str(), owner.c_str());
		bool rc = false;
		if (m_usePthreads) {
			pthread_mutex_lock(&m_pthread_mutex);
		} else {
			rc = ::xSemaphoreTake(m_semaphore, portMAX_DELAY) == pdTRUE;	// 這行代表它會一直等到拿到semaphore，然後回傳true
		}
		m_owner = owner;
		if (rc) {
>4>			ESP_LOGD(LOG_TAG, "Semaphore taken:  %s", toString().c_str());
		} else {
			ESP_LOGE(LOG_TAG, "Semaphore NOT taken:  %s", toString().c_str());
		}
		return rc;
	} // Semaphore::take

	std::string FreeRTOS::Semaphore::toString() {
		std::stringstream stringStream;
		stringStream << "name: "<< m_name << " (0x" << std::hex << std::setfill('0') << (uint32_t)m_semaphore << "), owner: " << m_owner;
		return stringStream.str();
	} // toString

>5>	11:22:50.100 -> [  1223][V][FreeRTOS.cpp:63] wait(): >> wait: Semaphore waiting: name: RegisterAppEvt (0x3ffd5fec), owner: registerApp for registerApp
/***corresponding code****/
	uint32_t FreeRTOS::Semaphore::wait(std::string owner) {
>5>		ESP_LOGV(LOG_TAG, ">> wait: Semaphore waiting: %s for %s", toString().c_str(), owner.c_str());

		if (m_usePthreads) {
			pthread_mutex_lock(&m_pthread_mutex);
		} else {
			xSemaphoreTake(m_semaphore, portMAX_DELAY);	// 這行代表它會一直等到拿到semaphore
		}

		m_owner = owner;

		if (m_usePthreads) {
			pthread_mutex_unlock(&m_pthread_mutex);		
		} else {
			xSemaphoreGive(m_semaphore);		// 這行代表它會釋放semaphore
		}

>C>		ESP_LOGV(LOG_TAG, "<< wait: Semaphore released: %s", toString().c_str());
		m_owner = std::string("<N/A>");
		return m_value;
	} // wait
	

>7>	11:22:50.134 -> [  1224][D][BLEDevice.cpp:102] gattServerEventHandler(): gattServerEventHandler [esp_gatt_if: 4] ... ESP_GATTS_REG_EVT
*8*	11:22:50.134 -> [  1245][V][BLEUtils.cpp:1530] dumpGattServerEvent(): GATT ServerEvent: ESP_GATTS_REG_EVT
*8*	11:22:50.134 -> [  1253][V][BLEUtils.cpp:1673] dumpGattServerEvent(): [status: ESP_GATT_OK, app_id: 0]
>9>	11:22:50.134 -> [  1261][V][BLEServer.cpp:144] handleGATTServerEvent(): >> handleGATTServerEvent: ESP_GATTS_REG_EVT
/***corresponding code****/	
	//下面這一個函式出現在BLEServer::registerApp(uint16_t m_appId)裡面，它會把BTC_GATTS_ACT_APP_REGISTER這個message經過一連串的傳遞，passing給BLEDevice::gattServerEventHandler。
	//並且這個函式，會在背景執行，也就concurrent running by using thread.
	esp_err_t esp_ble_gatts_app_register(uint16_t app_id)
	{
		btc_msg_t msg = {0};
		btc_ble_gatts_args_t arg;

		ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

		//if (app_id < ESP_APP_ID_MIN || app_id > ESP_APP_ID_MAX) {
		if (app_id > ESP_APP_ID_MAX) {
			return ESP_ERR_INVALID_ARG;
		}

		msg.sig = BTC_SIG_API_CALL;
		msg.pid = BTC_PID_GATTS;
		msg.act = BTC_GATTS_ACT_APP_REGISTER;
		arg.app_reg.app_id = app_id;

		return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gatts_args_t), NULL, NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
	}
	
	void BLEDevice::gattServerEventHandler(
	   esp_gatts_cb_event_t      event,
	   esp_gatt_if_t             gatts_if,
	   esp_ble_gatts_cb_param_t* param
	) {
>7>		ESP_LOGD(LOG_TAG, "gattServerEventHandler [esp_gatt_if: %d] ... %s",
			gatts_if,
			BLEUtils::gattServerEventTypeToString(event).c_str());	

*8*		BLEUtils::dumpGattServerEvent(event, gatts_if, param);		// 這個函式只是把serverEvent產生的log倒出來而已

		// ...省略case

		if (BLEDevice::m_pServer != nullptr) {
>9>			BLEDevice::m_pServer->handleGATTServerEvent(event, gatts_if, param);
		}

		if(m_customGattsHandler != nullptr) {
			m_customGattsHandler(event, gatts_if, param);
		}

	} // gattServerEventHandler
		

	void BLEServer::handleGATTServerEvent(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t* param) {
>9>		ESP_LOGD(LOG_TAG, ">> handleGATTServerEvent: %s",
			BLEUtils::gattServerEventTypeToString(event).c_str());

		switch(event) {
			// ESP_GATTS_ADD_CHAR_EVT - Indicate that a characteristic was added to the service.
			// add_char:
			// - esp_gatt_status_t status
			// - uint16_t          attr_handle
			// - uint16_t          service_handle
			// - esp_bt_uuid_t     char_uuid
			//
			
			// ......省略其他case

			// ESP_GATTS_REG_EVT
			// reg:
			// - esp_gatt_status_t status
			// - uint16_t app_id
			//
			case ESP_GATTS_REG_EVT: {
				m_gatts_if = gatts_if;
>6>				m_semaphoreRegisterAppEvt.give(); // Unlock the mutex waiting for the registration of the app.
				break;
			} // ESP_GATTS_REG_EVT

			// ......省略其他case

			default:
				break;
		}

		// Invoke the handler for every Service we have.
		m_serviceMap.handleGATTServerEvent(event, gatts_if, param);

>A>		ESP_LOGD(LOG_TAG, "<< handleGATTServerEvent");
	} // handleGATTServerEvent



	
>6>	11:22:50.167 -> [  1270][V][FreeRTOS.cpp:143] give(): Semaphore giving: name: RegisterAppEvt (0x3ffd5fec), owner: registerApp
>A>	11:22:50.167 -> [  1279][V][BLEServer.cpp:281] handleGATTServerEvent(): << handleGATTServerEvent
>C>	11:22:50.167 -> [  1279][V][FreeRTOS.cpp:77] wait(): << wait: Semaphore released: name: RegisterAppEvt (0x3ffd5fec), owner: <N/A>
>B>	11:22:50.199 -> [  1296][V][BLEServer.cpp:295] registerApp(): << registerApp



	11:22:50.199 -> [  1302][V][BLEDevice.cpp:83] createServer(): << createServer
	11:22:50.199 -> [  1307][V][BLEServer.cpp:67] createService(): >> createService - 4fafc201-1fb5-459e-8fcc-c5c9c331914b
	11:22:50.199 -> [  1316][V][FreeRTOS.cpp:189] take(): Semaphore taking: name: CreateEvt (0x3ffe0fc8), owner: <N/A> for createService
	11:22:50.230 -> [  1327][V][FreeRTOS.cpp:198] take(): Semaphore taken:  name: CreateEvt (0x3ffe0fc8), owner: createService
	11:22:50.230 -> [  1336][V][BLEService.cpp:60] executeCreate(): >> executeCreate() - Creating service (esp_ble_gatts_create_service) service uuid: 4fafc201-1fb5-459e-8fcc-c5c9c331914b
	11:22:50.230 -> [  1351][V][FreeRTOS.cpp:189] take(): Semaphore taking: name: CreateEvt (0x3ffe1954), owner: <N/A> for executeCreate
	11:22:50.264 -> [  1361][V][FreeRTOS.cpp:198] take(): Semaphore taken:  name: CreateEvt (0x3ffe1954), owner: executeCreate

