# Disclaimer
This repository contains code that I wrote when studying Software Engineering at Czech Technical University in Prague.

The objective of these assignments was to write code that works, covers all edge cases, and is somewhat performant. Usually, there were additional rules we had to follow - like limits on the amount of memory our programs could use or that all code needed to be in a single file 🤷‍♂️.

Please keep in mind that I wrote this code years ago and that it was a school assignment. This repository does not serve as the best example of my coding skills.

You can read the full assignment bellow (in Czech).

# Forex

Úkolem je realizovat třídu, která bude umožňovat rychle řešit problémy v oblasti bankovnictví.

Předpokládáme bankovní operace na finančních trzích. Banka sestavuje svůj kurzovní lístek, ve kterém pro vybrané dvojice měn vyplní směnný kurz. Banka má absolutní volnost, které dvojice měn a za jaký kurz bude směňovat. Tedy banka může nabízet např. směnu USD -> EUR, ale nemusí nabízet směnu EUR -> USD.

Směnné kurzy vždy zpracováváme na 4 desetinná místa (pips). Takto zaokrouhleným kurzem jsou přepočítávané všechny směny, které banka provádí pro své klienty. Uvnitř banky se ale používá kurz nezaokrouhlený, zaokrouhluje se pouze 1x po provedení všech směn uvnitř banky. Pokud tedy banka má kurzovní lístek např.:

```
CZK -> USD: 0.0398
USD -> EUR: 0.9259
EUR -> GBP: 0.7728
```

pak může klient požádat o převod CZK -> USD -> EUR -> GBP. Banka použije zveřejněné kurzy a jejich vynásobením dostane 0.02847831.., který pro klienta upraví na 4 desetinná místa (další řády odsekne, zaokrouhlí směrem dolů) na kurz 0.0284. Předpokládáme, že banka nevyžaduje další poplatky za směnu (poplatky za směnu jsou zahrnuty v kurzech).

Při zveřejnění kurzu si banka musí dát pozor, aby se postupnými konverzemi nedošlo k situaci, kde by se peníze daly "vyrobit" směnou v kruhu. Pokud by v uvedeném příkladu banka nabízela i směnu GBP -> CZK: 35.1263, pak by se konverzí CZK -> USD -> EUR -> GBP -> CZK došlo ke kurzu 1.0003378... (pro zákazníka 1.0003), tedy na vložené koruně by šlo vydělat 0.03 haléře (což pro velký objem vložených peněz a velké množství konverzí může být mnoho peněz). Takovému jevu se říká arbitrage. Problémem je zjistit, zda daný kurzovní lístek obsahuje arbitrage, případně jakými směnami lze k arbitrage dojít.

Zcela opačný problém řeší klienti bank. Ti mají k dispozici kurzovní lístky jednotlivých bank a potřebují převést své peníze z jedné měny do druhé. Vyberou si banku a té předají seznam požadovaných konverzí. Banka provede požadované konverze (viz pravidla výše) a podle vypočteného kurzu proplatí peníze. Banka například může nabízet následující kurzy:

```
CZK -> USD: 0.0398
USD -> EUR: 0.9259
EUR -> GBP: 0.7728
CZK -> GBP: 0.0280
```
Pokud bychom chtěli převést CZK na GBP, mohli bychom použít přímo kurz 0.0280 nebo použít konverzi CZK -> USD -> EUR -> GBP a dosáhnout lepšího kurzu 0.0284.
Vaším úkolem je realizovat třídu CConsultant, která dokáže takové problémy řešit. Oba uvedené problémy jsou výpočetně náročnější a oba problémy je potřeba řešit rychle (kurzy se mění). Proto bude využito vláken k rozdělení výpočetní zátěže na více CPU a asynchronního modelu výpočtu.

Třída CConsultant má modelovat finančního poradce, který přebírá problémy k řešení, řeší je a po vyřešení informuje o dokončení výpočtu. Pro načítání problémů si vytvořte dvě pomocná vlákna. Vlákna budou volat zadané funkce, jedno vlákno bude volat funkci pro doručování problémů typu arbitrage, druhé pro doručování problémů hledání výhodného kurzu. Zadaný problém má podobu instance třídy CArbitrage nebo třídy CExchange (podle typu úlohy). Pomocná vlákna předají získané instance problémů vláknům pracovním. Vlákna, která přebírají zadávané problémy, nejsou určena k tomu, aby počítala řešení, jejich úkolem je pouze předání problémů dále.

Pracovních vláken vytvoříte více (podle parametrů při inicializaci). Pracovní vlákna vyřeší zadanou instanci problémů a podle výsledků vyplní příslušné složky instance CArbitrage / CExchange. Po vyplnění informují zadavatele o dokončení výpočtu dané instance problému tím, že zavolají notifikační funkci s identifikací vyřešené instance problému (identifikátor je součástí předané instance CArbitrage / CExchange).

Rozhraním vaší implementace bude třída CConsultant. V této třídě musí být k dispozici metody podle popisu níže (mohou existovat i další privátní metody potřebné pro Vaší implementaci):

**konstruktor(arbFn,xchgFn,complFn)**
inicializuje instanci. Má parametry:
- arbFn - ukazatel na funkci, která po zavolání dodá další instanci problému arbitrage k vyřešení. Vaše implementace bude tuto funkci volat opakovaně, každé zavolání dodá další instanci třídy CArbitrage (popis níže). Zadávání problémů CArbitrage skončí v okamžiku, kdy volání této funkce vrátí hodnotu NULL.
- xchgFn - ukazatel na funkci, která po zavolání dodá další instanci problému nalezení nejvýhodnějšího způsobu směny peněz. Vaše implementace bude tuto funkci volat opakovaně, každé zavolání dodá další instanci třídy CExchange (popis níže). Zadávání problémů CExchange skončí v okamžiku, kdy volání této funkce vrátí hodnotu NULL.
- complFn - ukazatel na funkci, kterou Vaše implementace zavolá po vyřešení každého jednoho zadaného problému (CArbitrage / CExchange). Funkci předáte v parametru identifikátor vyřešeného problému (tento identifikátor dostanete jako členskou proměnnou v CArbitrage/CExchange).

**Execute (thr)**
- Metoda spustí vlastní výpočet. V této metodě vytvoříte potřebná pracovní vlákna pro výpočty a vlákna, která budou načítat vstupy. Pracovních vláken vytvořte celkem thr podle hodnoty parametru. Metoda Execute skončí v okamžiku, kdy vyřešíte všechny problémy ze vstupu (tedy volání obou načítacích funkcí vrací NULL).
- Bylo zmíněno, že metoda Execute vytvoří thr+2 vláken - dvě vlákna budou načítat vstup voláním předaných ukazatelů na funkce. Fakticky ale stačí vytvořit pouze threads + 1 nových vláken a využít i vlákna. ze kterého byla volaná Execute.
- Metoda Execute skončí v okamžiku, kdy byly vyřešené všechny zadané vstupní problémy a další nové problémy nejsou k dispozici (volání jak arbFn, tak xchgFn vrací NULL). Po zaniknutí Vámi vytvořených pracovních vláken metoda Execute vrátí řízení volanému. Neukončujte celý program (nevolejte exit a podobné funkce), pokud ukončíte celý program, budete hodnoceni 0 body.

**ArbitrageSeq(arb)**
- metoda vypočte sekvenčně jeden zadaný problém typu CArbitrage (parametr). Testovací prostředí nejprve zkouší sekvenční řešení, abyste případně snáze odhalili chyby v implementaci algoritmu.

**ExchangeSeq(xchg)**
- metoda vypočte sekvenčně jeden zadaný problém typu CExchange (parametr). Testovací prostředí nejprve zkouší sekvenční řešení, abyste případně snáze odhalili chyby v implementaci algoritmu.
Třída CArbitrage je deklarovaná a implementovaná v testovacím prostředí. Pro testování Vaší implementace je dispozici v bloku podmíněného překladu (ponechte jej tak). Význam složek je následující:

**m_ProblemID**
jednoznačný identifikátor zadaného problému. Tento identifikátor předejte jako parametr funkci, kterou oznámíte vyřešení této instance problému.

**m_Rates**
C++ řetězec se seznamem převáděných měn a jejich kurzů. Řetězec je čárkami oddělený seznam dvojic měn (měna je vždy zadána zkratkou z velkých písmen) a kurzu (desetinné číslo). Přesný popis je dán EBNF níže, intuitivně je formát zřejmý z ukázek.

**m_Arbitrage**
příznak, který vyplníte na základě Vašeho výpočtu. Bude nastaven na true, pokud naleznete arbitrage.

**m_Currency**
tuto složku vyplňujte pouze pokud narazíte na arbitrage. Pak bude obsahovat posloupnost názvů měn jejichž konverzí lze dosáhnout arbitrage. Tedy např. pro příklad z ukázky (cyklus CZK -> USD -> EUR -> GBP -> CZK) bude složka vyplněna 5 řetězci v tomto pořadí: "CZK", "USD", "EUR", "GBP" a "CZK". Pokud existuje více možností výběru měn pro arbitrage, můžete do výsledku uložit libovolnou takovou sekvenci.

**m_Rate**
složku vyplňujte pouze pokud nastane arbitrage. Bude mít hodnotu kurzu, kterého se dosáhne cyklickou výměnou měn podle m_Currency. Protože se má jednat o arbitrage, musí být kurz větší než 1.0 (vzhledem k pravidlům zaokrouhlení dokonce větší-roven 1.0001).

Třída CExchange je deklarovaná a implementovaná v testovacím prostředí. Pro testování Vaší implementace je dispozici v bloku podmíněného překladu (ponechte jej tak). Význam složek je následující:

**m_ProblemID**
jednoznačný identifikátor zadaného problému. Tento identifikátor předejte jako parametr funkci, kterou oznámíte vyřešení této instance problému.

**m_From**
jméno měny, ze které chceme převádět.

**m_To**
jméno měny, na kterou chceme převádět.

**m_Banks**
seznam kurzovních lístků bank, které mohou převod realizovat. Pro každou banku obsahuje řetězec s formátem, který odpovídá formátu ze zadání problémů CArbitrage.

**m_Result**
příznak, zda lze požadovanou směnu vůbec realizovat (tj. zda v seznamu existuje banka, která umožní postupnými konverzemi dojít od měny m_From ke měně m_To. Pokud lze konverzi uskutečnit, nastavte příznak hodnotu true.

**m_BankIdx**
pokud lze směnu realizovat, vyplňte tuto složku na pořadové číslo banky, kde lze dosáhnout nejlepší kurz. Indexy počítáme od 0.

**m_Currency**
pokud lze směnu realizovat, vyplňte tuto složku na posloupnost názvů měn, kterými se lze dostat k nejlepšímu směnnému kurzu (v bance m_BankIdx). První hodnota v poli musí být shodná s m_From, poslední s m_To.

**m_Rate**
pokud lze směnu realizovat, nastavte tuto složku na hodnotu nejlepšího směnného kurzu (musí odpovídat sekvenci konverzí podle m_Currency v bance m_BankIdx). Vypočtený kurz zaokrouhlete podle pravidel (zaokrouhlení dolů na 4 desetinná místa).

### Formát kurzovních lístků
Následující EBNF jej popisuje exaktně:
```
rates  ::=  ws rate { ws ',' ws rate } ws
rate   ::=  cur ws '->' ws cur ws ':' ws val
cur    ::=  letter { letter }
val    ::=  num ['.' [num] ] [ 'e'  [ '+' | '-' ] num ]  | '.' num [ 'e'  [ '+' | '-' ] num ]
num    ::=  digit { digit }
digit  ::=  '0' | '1' | '2' | ... | '9'
letter ::= 'A' | 'B' | 'C' | ... | 'Z' | 'a' | 'b' | ... | 'z'
ws     ::= { ' ' | '\t' | '\r' | '\n' }
```
