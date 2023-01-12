# Monopoly-Banking

Monopoly Banking selber nachgebaut und programmiert für die Projektwoche in 01/2023


# Übersicht

Wir arbeiten in einer Gruppe von 4 Leuten, 2 sind für das Design und 2 sind für das Programmieren zuständig.
Die Grundidee ist, dass wir Monopoly Banking selber nachbauen, mit einem Arduino und NFC-Karten. Das Board wird entweder aus einem alten Brettspiel oder aus Pappe bestehen und mit eigen erstellten Straßen und Ereigniskarten erstellt. Die Straßen und Karten werden Referenzen an unsere Schule, Lehrer und Schüler haben.


# Funktionsweise

## Banking System

Das Banking System wird mit einem [Arduino](https://www.arduino.cc/), einem [PN532 NFC RFID Lese-/Schreibgerät](https://funduinoshop.com/elektronische-module/wireless-iot/rfid-nfc/pn532-nfc-rfid-v3-modul-fuer-arduino-und-co.), einem Ziffernblatt und einem LCD verwirklicht.
Der Arduino fungiert als Datenbank und "Gehirn" der Bank. Ein Spieler wählt auf dem Ziffernblatt, ob dieser Geld schicken möchte, oder seinen Kontostand abfragen will, hält dann die NFC-Karte an das NFC-Lesegerät und folgt den Anweisungen auf dem Display.
Der Geldbetrag und die ganze Logik erfolgt auf dem Arduino, mit einer im Runtime erstellten Datenbank, die beim Neustarten des Arduinos neu erstellt wird.

## Geld verschicken / empfangen

Will ein Spieler A einem anderen Spieler B Geld schicken, weil dieser eine Straße verhandelt hat oder auf ein Grundstück gekommen ist, so drückt A eine Taste auf dem Ziffernblatt und hält die Karte an das Lesegerät. Danach hält B die Karte dran. Spieler A gibt dann den Betrag ein und bestätigt.
Sollte ein Spieler aufgrund von Ereigniskarten Geld in den Pott legen müssen, wird es genauso wie oben ausgeführt, nur anstatt Spieler B's Karte, wird die Karte vom Pott dran gehalten. Das gleiche, wenn Spieler A Geld an die Bank geben muss.

## Geld vom Pott einsammeln

Wenn ein Spieler auf das "Frei Parken" Feld kommt, darf dieser den gesamten Pott einsammeln. Dafür hält man erst die Karte vom Pott an das Lesegerät, und dann die Karte vom Spieler. Daraufhin erscheint eine Zahl auf dem LCD, die angibt, wie viel Geld im Pott ist. Der Spieler bestätigt die Transaktion mit einer Taste.
