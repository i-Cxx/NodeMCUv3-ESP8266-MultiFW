Okay, ich verstehe deine Neugierde und deinen Wunsch, das Beste aus deiner aktuellen Einrichtung herauszuholen!

Da ich Bilder nicht direkt "sehen" kann, bitte ich dich, das Foto deines Aufbaus kurz zu beschreiben (wie ist der Draht genau am Breadboard/NodeMCU angeschlossen, gibt es noch andere Komponenten?).

### Was misst dein "EMF-Gerät" mit dem Draht an A0?

Dein aktuelles Setup mit dem langen Kupferdraht direkt an A0 ist, wie gesagt, kein kalibriertes EMF-Messgerät. Es misst in erster Linie **elektromagnetisches Rauschen und Störungen** in deiner Umgebung.

Du kannst damit feststellen, ob:

1.  **Ein starkes, wechselndes elektrisches Feld in der Nähe ist:**
    * Zum Beispiel, wenn du den Draht in die Nähe von eingeschalteten Stromkabeln, Lampen, Netzteilen, Monitoren oder anderen Geräten mit Netzspannung bewegst. Die 50/60 Hz-Wechselspannung erzeugt ein starkes elektrisches Feld, das im Draht induziert wird.
    * Du siehst eine **relative Änderung** der Werte: Näher = höhere Werte, weiter weg = niedrigere Werte.
2.  **Jemand (oder du selbst) den Draht berührt oder sich nähert:**
    * Unser Körper wirkt wie eine große Antenne und kann elektrische Ladungen oder Störungen in den Draht induzieren. Die Werte werden sich ändern, wenn du den Draht berührst oder einfach deine Hand in die Nähe hältst.
3.  **Starkes hochfrequentes Rauschen (RF) vorhanden ist:**
    * WLAN, Bluetooth, Mobilfunk – der Draht kann diese auch aufnehmen. Die Werte können sich ändern, wenn du dein Handy direkt daneben legst oder eine WLAN-Übertragung stattfindet.

**Zusammenfassend:** Es ist ein **sehr grober Indikator für die Anwesenheit von elektrischem/elektromagnetischem Rauschen**, kein präzises Messgerät. Du kannst damit relative Veränderungen feststellen, aber keine absoluten Werte in wissenschaftlichen Einheiten.

---

### Tipps für eine "Antenne" zum Messen von Magnetfeldern (oder ähnlichem) mit deinem 3mm Kupferdraht

Für Magnetfelder brauchst du in der Regel eine **Spule** (eine Schleife oder Windung von Draht). Ein einzelner gerader Draht ist primär für elektrische Felder oder hochfrequente elektromagnetische Wellen (als Antenne) geeignet.

**1. Einfache Spule für Magnetfelder (ELF - Niederfrequenz):**

* **Was du tun kannst:** Nimm deinen 130 cm langen 3mm Kupferdraht. Wickle ihn zu einer **Spule** auf. Je mehr Windungen (Umdrehungen) du hast und je größer die Fläche der Spule ist, desto empfindlicher wird sie für Magnetfelder.
    * **Beispiel:** Wickle den Draht fest um einen runden Gegenstand (z.B. eine Getränkedose, eine PVC-Röhre, ein Papprollen-Kern). Versuche, so viele Windungen wie möglich auf relativ kleinem Raum zu bekommen, um eine hohe Windungszahl zu erreichen. Eine Spule mit 10-20 Windungen (oder mehr, wenn der Draht lang genug ist) ist besser als nur eine einzelne Schleife.
* **Wie es funktioniert:** Wenn sich ein Magnetfeld (z.B. von einem Trafo, Motor, Stromkabel) durch die Spule ändert, wird eine Spannung in der Spule induziert. Diese Spannung ist proportional zur Stärke und Änderungsrate des Magnetfelds sowie zur Windungszahl und Fläche der Spule.
* **Limitationen:** Auch diese induzierte Spannung wird **extrem klein** sein (Mikro- oder Millivoltbereich). Dein ESP8266 ADC wird sie immer noch kaum oder gar nicht direkt erkennen können. Du wirst weiterhin eine **Verstärker- und Gleichrichterschaltung** benötigen, um ein nutzbares Signal für A0 zu erhalten. Ohne diese Schaltung misst du auch mit einer Spule hauptsächlich Rauschen.

**2. Verbesserte Spule mit Ferritkern (falls du einen findest):**

* **Was du tun kannst:** Wenn du irgendwo einen Ferritstab (wie in alten Radios für die Antenne) oder einen Ferritring finden könntest, wickel deinen Kupferdraht darum. Ferritmaterialien konzentrieren Magnetfelder sehr gut.
* **Effekt:** Eine Ferritkernspule ist deutlich empfindlicher für Magnetfelder als eine Luftspule gleicher Größe.
* **Limitationen:** Auch hier ist **Verstärkung und Gleichrichtung unerlässlich**.

**3. "Etwas anderes": Elektrische Felder (ELF - Niederfrequenz):**

* **Was du tun kannst:** Statt einer Spule könntest du eine größere leitende Fläche als Sensor verwenden. Zum Beispiel, wenn du ein Stück Aluminiumfolie (ca. 10x10 cm oder größer) hast, befestige den Kupferdraht daran und schließe es an A0 an.
* **Wie es funktioniert:** Eine größere Fläche ist empfindlicher für elektrische Felder (z.B. von statischer Elektrizität oder sich ändernden elektrischen Spannungen in der Umgebung).
* **Limitationen:** Sehr empfindlich gegenüber Umgebungsrauschen und der Nähe von Menschen. Benötigt ebenfalls **Verstärkung und Filterung** für sinnvolle Messungen.

**Zusammenfassende Empfehlung "ohne viel Aufwand":**

Ohne zusätzliche Elektronik (Verstärker, Gleichrichter, Filter) wirst du mit deinem NodeMCU und dem Kupferdraht immer nur ein **Rauschdetektor** sein, der auf starke Störquellen reagiert, aber keine präzisen oder quantifizierbaren EMF-Messungen liefert. Der Aufwand für eine **funktionierende** EMF-Messung liegt leider weniger in der Antenne selbst als in der Elektronik, die das winzige und oft AC-förmige Signal der Antenne für den ADC des ESP8266 nutzbar macht.

Der einfachste Weg, um *qualitativ* etwas mehr zu sehen, wäre, den Draht zu einer **Spule** zu wickeln (wie unter Punkt 1 beschrieben) und damit verschiedene Elektrogeräte abzuscannen. Du würdest dann immer noch die Peaks im Rauschen sehen, die von den Geräten erzeugt werden.

Um einen Schritt weiter zu gehen, müsstest du dir wirklich die grundlegenden Schaltungen mit einem **Op-Amp (z.B. LM358)** und ein paar Dioden/Kondensatoren ansehen. Das ist der Punkt, an dem "ohne viel Aufwand" aufhört und "Grundlagen der analogen Elektronik" beginnt.