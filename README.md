# Gestionarea-Traficului
### Protocolul cuprinde urmatoarele comenzi:
Toate comenzile sunt executate de server, clientul doar le citeste.<br />
`login : username` (verificarea existentei username-ului in baza de date se face
face de catre server, in cazul in care username-ul exista deja utilizatorul este
conectat cu succes, iar in caz contrar i se va solicita sa isi faca un cont)<br />
`register : username` (in cazul in care username-ul exista deja, utilizatorului
i se va trimite un mesaj de avertizare, daca nu, i se va trimite un mesaj de
confirmare)<br />
`logout` (utilizatorul va fi deconectat)<br />
`quit` (va deconecta clientul de la server)<br />
`help` (afiseaza sintaxa si descrierea comenzilor disponibile)<br />
### Urmatoarele comenzi vor putea fi executate doar daca clientul este conectat
la sistem.
`delete-account` (contul utilizatorului conectat va fi sters)<br />
`speed : < viteza >` (fiecare client va fi instiintat de client ca trebuie sa trimita viteza cu care circula cu o frecventa de un minut. Server-ul va verifica daca
viteza este regulamentara)<br />
`accident : < street >` (fiecare client poate raporta un accident pe care serverul il va inregistra in baza de date si va trimite un mesaj catre toti utilizatorii
conectati la sistem)<br />
`no-accident : < street >` (fiecare client care observa ca pe o anumita strada
accidentul a fost solutionat, poate sterge accidentul din sistem)<br />
`all-accidents` (pentru a vedea toate accidentele raportate)<br />
`traffic-jam : < street >` (pentru a raporta un ambuteiaj)<br />
`no-traffic-jam : < street >` (pentru a sterge un ambuteiaj)<br />
`all-traffic-jams` (pentru a vedea toate ambuteiajele raportate)<br />
`newsletter : < weather >, < sport >, < fuel >` (clientul poate solicita abonarea
la una sau mai multe categorii, iar serverul ii va trimite informatii despre op-
Raport Tehnic 5
tiunea aleasa)<br />
`get-directions : < startStreet >→< destinationStreet >` (pentru a vedea
cea mai scurta ruta dintre startStreet si destinationStreet)
