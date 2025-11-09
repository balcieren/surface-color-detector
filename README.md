# ESP32 TCS3200 Renk Sensörü Projesi

Bu proje, ESP32 ve TCS3200 renk sensörü kullanarak yüzey renklerini algılayan basit bir uygulamadır.

## Donanım Gereksinimleri

- ESP32 Geliştirme Kartı
- TCS3200 Renk Sensörü Modülü
- Jumper Kablolar

## Bağlantı Şeması

| TCS3200 | ESP32 |
|---------|-------|
| VCC     | 3.3V  |
| GND     | GND   |
| S0      | GPIO4 |
| S1      | GPIO5 |
| S2      | GPIO18|
| S3      | GPIO19|
| OUT     | GPIO21|

## Çalışma Prensibi

TCS3200 sensörü, yüzeydeki rengi algılar ve RGB (Kırmızı, Yeşil, Mavi) değerlerini frekans olarak döndürür.

- **S0/S1**: Frekans ölçeklendirme (bu projede %20'ye ayarlı)
- **S2/S3**: Renk filtre seçimi (Kırmızı, Yeşil, Mavi, Net)
- **OUT**: Seçilen renk için frekans çıkışı

### Renk Filtreleri

| S2 | S3 | Renk Filtresi |
|----|----|--------------|
| LOW| LOW| Kırmızı      |
| HIGH|HIGH| Yeşil        |
| LOW|HIGH| Mavi         |
| HIGH|LOW| Net (Clear)  |

## Özellikler

✅ RGB renk değerlerini okur
✅ Basit renk algılama (Kırmızı, Yeşil, Mavi, Beyaz)
✅ LED ile görsel geri bildirim
✅ Seri monitörde detaylı çıktı

## Kullanım

1. Projeyi PlatformIO ile derleyin:
   ```bash
   pio run
   ```

2. ESP32'ye yükleyin:
   ```bash
   pio run --target upload
   ```

3. Seri monitörü açın (115200 baud):
   ```bash
   pio device monitor
   ```

4. Sensörü farklı renkli yüzeylere tutun ve sonuçları gözlemleyin.

## LED Geri Bildirimi

- 1 yanıp sönme: Kırmızı algılandı
- 2 yanıp sönme: Yeşil algılandı
- 3 yanıp sönme: Mavi algılandı
- 4 yanıp sönme: Belirsiz/Beyaz

## Kalibrasyon

Her TCS3200 sensörü farklı değerler verebilir. Daha iyi sonuçlar için:

1. Bilinen renklere sensörü tutun
2. Seri monitördeki değerleri kaydedin
3. `detectColor()` fonksiyonundaki eşik değerlerini ayarlayın

## Notlar

- Sensör, ortam ışığından etkilenebilir
- En iyi sonuçlar için sensörü yüzeye 1-2 cm yaklaştırın
- Parlak veya mat yüzeyler farklı sonuçlar verebilir

## Lisans

MIT License - İstediğiniz gibi kullanabilirsiniz!
