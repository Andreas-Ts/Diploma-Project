# Code that was created for the Bachelor thesis "Indoor pollutant location detection using sensor networks and machine learning"

The presentation and whole text are given at the [Παρουσίαση Διπλωματικής εργασίας] και (Παρουσίαση Διπλωματικής εργασίας.pdf)[Διπλωματική Εργασία Ανδρέας Τσιρώνης](Διπλωματική Εργασία Ανδρέας Τσιρώνης.pdf).
Below, the Greek and English abstract it will be showcased.

## Περίληψη

Η ποιότητα του εσωτερικού αέρα (Indoor Air Quality,IAQ) αποτελεί ένα πολύ
σημαντικό κομμάτι της υγείας και ποιότητας ζωής των ανθρώπων,ειδικά στην σημερινή
επόχη. Χάρη στους αισθητήρες αέρα φτηνού κόστους (Low Cost gas Sensors,LCS), που
έχουν αναπτυχθεί πολύ την τελευταία δεκαπενταετία,μας δίνει την δυνατότητα να
μελετήσουμε τον IAQ με πιο προσιτό τρόπο και σε μεγαλύτερη κλίμακα. Ένα σημαντικός
παράγοντας του IAQ είναι οι πηγές ρύπου που μπορεί να υπάρχουν στον εσωτερικό
χώρο,με την άμεση αντιμετώπιση τους να είναι ένα από τους καλύτερους τρόπους
βελτιώσεις του IAQ. Σε αυτή την εργασία επιχειρήσαμε να εξερευνήσουμε ένα τομέα που
δεν έχει ερευνηθεί έως τώρα από κάποια άλλη έρευνα,την εύρεση της θέσης μίας πηγής
ρύπου στον εσωτερικό χώρο, χρησιμοποιώντας αισθητήρες αέρα φτηνού κόστους.

Αναπτύξαμε ένα δίκτυο αισθητήρων Intertet Of Things (IoT),με τους μικροελεκτές
ESP32 και τους αισθητήρες BME680 που έχει LCS Ημιαγωγό Μεταλλικού Οξειδίου
(Metal Oxide Semiconductors,MOS), χρησιμοποιώντας το Bosch Software Environmental
Cluster (BSEC) για την εξαγωγή των τιμών breath VOC. Πραγματοποιήσαμε μία σειρά
πειραμάτων σε όλο το εύρος ενός δωματίου σπιτιού, χρησιμοποιώντας φαρμακευτικό
αλκοόλ και αιθανόλη σαν ρύπο και πηγή που εξετάζαμε. Χρησιμοποιήσαμε μηχανικά
μοντέλα παλινδρόμησης και τριπλευρικό εντοπισμό για την εύρεση της θέσης της πηγή
ρύπου μες στο χώρο. Δεν κατάφεραν οι αισθητήρες να δώσουν αποτελέσματα που να
ξεχωρίζουν τις διαφορετικές θέσεις της πηγής ρύπου σε όλο το δωμάτια,αλλά είχαμε
επιτυχημένες περιπτώσεις για υποομάδες πειραμάτων, όπου η υπερευαισθησία των
MOS αισθητήρων στους ρύπους μπορούσε να ξεχωρίσει ,μας κάνει αρκετά αισιόδοξους
για μελλοντικές μελέτες

## Abstract

Indoor Air Quality (IAQ) is a very important aspect of human health and qualityof life, especially in the modern era. Thanks to low-cost gas sensors (LCS), which havedeveloped significantly over the past fifteen years, it has become possible to study IAQin a more affordable way and on a larger scale. An important factor affecting IAQ is thepresence of pollutant sources in indoor environments. Their prompt identification andmitigation is one of the most effective ways to improve IAQ. In this study, we attemptedto explore a field that has not yet been investigated by previous research: the localizationof a pollutant source within an indoor environment using low-cost air sensors.We developed an Internet of Things (IoT) sensor network using ESP32 microcontrollersand BME680 sensors equipped with Metal Oxide Semiconductor (MOS) low-cost gassensing elements. We used the Bosch Software Environmental Cluster (BSEC) library to extract breath VOC values. A series of experiments was conducted throughout theentire area of a room in a residential house, using medical alcohol and ethanol as the pollutant and source under investigation. We applied mechanical regression models andtrilateration techniques to estimate the location of the pollutant source within the space.Although the sensors were not able to produce results that clearly distinguished between different pollutant source positions across the entire room, there were successful cases within subgroups of experiments. The high sensitivity of the MOS sensors to pollutants allows us to remain optimistic about future research in this field.


## File Structure

├───dataAnalysis and machine learning # Data transformation, data mining and machine learning pipelines
├───dataReceiverServer # Everything regarding the web application, except the database and the Grafana local server
├───include # The header files used for the ESP32 microcontrollers code.Alls the libraries are being united into the custom_headers.h
├───src # code of the ESP32 microcontrollers
