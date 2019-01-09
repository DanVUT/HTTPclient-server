# **IPK PROJ 1**
### **Meno:** Daniel Florek
### **Login:** xflore02
### **Popis:** *Jednoducha klient-server aplikacia komunikujuca pomocou TCP a Restful API*
### **Implementacny jazyk:**  *C++*
### **REST prikazy:**
#####  GET  *-stiahnutie suboru zo serveru*
#####  DELETE *-zmazanie suboru zo serveru*
#####  PUT  *-nahranie suboru na server* 

## **Client ftrest:**
#### **Parametre spustenia:**
##### ./ftrest [COMMAND] [REMOTE PATH] [LOCAL PATH]
##### __COMMAND:__
##### - lst = ls
##### - mkd = mkdir
##### - rmd = rmdir
##### - del = rm
##### - get = "download"
##### - put = "upload"
##### __REMOTE PATH:__
##### - http://server:port/user/path
### **Popis:**
##### Nazaciatku su spracovane argumenty z ktorych sa vytiahne prikaz, adresa servera, port, cesta na serveri a pripadne este cesta k lokalnemu suboru.
##### Nasledne sa na zaklade tychto informacii vygeneruje HTTP hlavicka s nejakymi dodatocnymi atributmi.
##### Vytvori sa TCP spojenie za pomoci BSD socketov. Po uspesnom naviazani spojenia sa odosle vygenerovana hlavicka serveru.
##### Klient nasledne caka na odozvu zo strany servera. Klient prijme odozvu zo servera vo forme hlavicky. Na zaklade odoslaneho prikazu na server a jeho validity sa ocakava v hlavicke jeden z nasledujucich HTTP kodov **200 OK**, **404 Not Found** alebo **400 Bad Request**. V pripade chyby sa z hlavicky vytiahne chybova hlaska a vypise sa na standartny chybovy vystup.     

    
## **Server ftrestd:**
#### **Parametre spustenia:**
##### ./ftrest [-r REMOTE PATH] [-p PORT]
##### __PORT:__
##### Ak nie je zadany prepinac -p tak port je automaticky nastaveny na _6677_
##### __REMOTE PATH:__
##### Ak nie je zadany prepinac -r tak cesta je nastavena na priecinok kde sa nachadza server
### **Popis:**
##### Server je implementovany iterativne. Nacuva na zadanom porte a caka na spojenie s klientom. Po nadviazani spojenia prijme hlavicku, rozparsuje ju a vytiahne si z nej potrebne udaje. Nasledne zadanu ulohu klientom vykona a odosle mu naspat vygenerovanu hlavicku s navratovym kodom. V pripade chyby sa k tejto hlavicke prida aj prisluchajuca chybova hlaska.
