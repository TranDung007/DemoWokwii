# SMART HOME ESP32 - KIEM TRA MQ-2 LIVE

Ban nay bo nut TEST GAS, doc DO GPIO35 moi vong loop, va bat/tat buzzer ngay khi DO doi trang thai. Khong co bo loc 3 mau.

1. Dung Wokwi, chay `pio run`, kiem tra SUCCESS, sau do khoi dong LAI Wokwi de nap firmware moi.
2. Serial hien DO moi 2 giay, va in them thong bao moi khi DO thay doi.
3. Keo GAS PPM tu thap den cao KHI WOKWI DANG CHAY. Neu DO khong doi, loi co the o phan mo phong MQ-2 hoac duong day, khong phai do debounce trong firmware.
4. Ten pin dung theo tai lieu Wokwi: gas:DO, gas:AO. Threshold 0.75 chi la gia tri thu nghiem, KHONG duoc dung lam nguong an toan.
5. Neu gas PPM thay doi ma DO giu nguyen, hay thu vi du MQ-2 digital chinh thuc tren docs.wokwi.com/parts/wokwi-gas-sensor de doi chieu bo mo phong.

MQ-2 tren phan cung that dung 5V, phai ha muc tin hieu DO de bao ve ESP32.
