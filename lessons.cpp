#include "lessons.h"

namespace {
QString choose(bool cs, const char *en, const char *cz) { return QString::fromUtf8(cs ? cz : en); }
QString sources(Fractal kind, bool cs) {
    QString links;
    switch (kind) {
    case Fractal::Mandelbrot:
        links = "<a href='https://people.math.harvard.edu/~ctm/home/text/others/mandelbrot/mandelbrot_set/mandelbrot_set.pdf'>B. Mandelbrot · Fractal aspects of iteration (1980)</a>"; break;
    case Fractal::Julia:
        links = "<a href='https://www.numdam.org/item/JMPA_1918_8_1__47_0/'>G. Julia · Mémoire sur l’itération des fonctions rationnelles (1918)</a>"; break;
    case Fractal::BurningShip:
        links = "<a href='https://doi.org/10.1016/0097-8493(92)90032-Q'>M. Michelitsch &amp; O. E. Rössler · The burning ship and its quasi-Julia sets (1992)</a><br><br><a href='https://mathr.co.uk/helm/AtTheHelmOfTheBurningShip-Paper.pdf'>C. Heiland-Allen · At the helm of the Burning Ship (2019)</a>"; break;
    case Fractal::Tricorn:
        links = "<a href='https://www.math.stonybrook.edu/preprints/ims90-6.pdf'>J. Milnor · Remarks on Iterated Cubic Maps (1992)</a>"; break;
    }
    return "<hr><h3>" + choose(cs,"Read the sources","Původní zdroje") + "</h3><p>" + links + "</p><p class='note'>"
        + choose(cs,"Lessons work offline. Source links open in your browser and need an internet connection.",
                    "Texty fungují bez internetu. Odkazy na zdroje se otevírají v prohlížeči a vyžadují připojení.") + "</p>";
}
}
QString explorationText(Fractal kind, bool cs) {
    switch (kind) {
    case Fractal::Mandelbrot: return choose(cs,"Explore a branching boundary. Zoom into a curl, then increase the iteration limit to compare its detail.","Prozkoumejte rozvětvený okraj. Přibližte spirálu a zvyšte počet iterací. Porovnejte podrobnosti.");
    case Fractal::Julia: return choose(cs,"Meet the basilica: c = −1. Follow 0 → −1 → 0, then compare this set with the other Julia presets.","Poznejte baziliku: c = −1. Sledujte 0 → −1 → 0 a porovnejte tento tvar s ostatními předvolbami.");
    case Fractal::BurningShip: return choose(cs,"Visit a small ship near c = −1.76 − 0.035i. Look for masts and compare their shapes at different scales.","Navštivte malou loď poblíž c = −1,76 − 0,035i. Hledejte stěžně a porovnejte jejich tvary při různém přiblížení.");
    case Fractal::Tricorn: return choose(cs,"Explore the left arm. Reset the view to compare all three arms, and look for smaller related shapes.","Prozkoumejte levé rameno. Obnovte výchozí pohled, porovnejte všechna tři ramena a hledejte menší podobné tvary.");
    }
    return {};
}
QString lessonHtml(Fractal kind, int tab, bool cs) {
    QString body;
    if (tab == 0) {
        body = "<h1>" + choose(cs,"Small rule. Endless detail.","Malé pravidlo. Nekonečné detaily.") + "</h1><p>"
          + choose(cs,"A fractal reveals structure at many scales. Here, each pixel represents a complex number: a horizontal real part and a vertical imaginary part. Repeating a rule tells us how to color it.",
                      "Fraktál odhaluje strukturu na mnoha úrovních přiblížení. Každý pixel zde představuje komplexní číslo: vodorovnou reálnou a svislou imaginární část. Barvu určuje opakované použití pravidla.") + "</p>";
        switch (kind) {
        case Fractal::Mandelbrot:
            body += choose(cs,"<h2>A map of possibilities</h2><p>Choose a pixel and call its number <b>c</b>. Start at zero, square the current number, add c, and repeat. Some paths stay near the origin; others escape.</p><p>For c = 0, the path stays at zero. For c = 1, it goes 0 → 1 → 2 → 5 → 26 and grows rapidly. The intricate border separates these behaviors.</p>",
                              "<h2>Mapa možností</h2><p>Vyberte pixel a jeho číslo označte <b>c</b>. Začněte nulou, aktuální číslo umocněte na druhou, přičtěte c a opakujte. Některé dráhy zůstávají poblíž počátku, jiné uniknou.</p><p>Pro c = 0 zůstává dráha v nule. Pro c = 1 dostanete 0 → 1 → 2 → 5 → 26 a rychlý růst. Složitý okraj odděluje tato chování.</p>"); break;
        case Fractal::Julia:
            body += choose(cs,"<h2>One rule, many starting points</h2><p>Keep <b>c</b> fixed for the whole picture. Each pixel now supplies the starting number z₀. Changing c changes the entire landscape.</p><p>The dark region approximates the <b>filled Julia set</b>. Its boundary is the Julia set itself. Try the presets to discover how one formula can produce very different shapes.</p>",
                              "<h2>Jedno pravidlo, mnoho počátků</h2><p>Pro celý obraz zůstává <b>c</b> stejné. Každý pixel nyní určuje počáteční číslo z₀. Změna c promění celou krajinu.</p><p>Tmavá oblast přibližuje <b>vyplněnou Juliovu množinu</b>. Její hranice je samotná Juliova množina. Vyzkoušejte předvolby: stejný vzorec vytváří velmi odlišné tvary.</p>"); break;
        case Fractal::BurningShip:
            body += choose(cs,"<h2>Fold before you square</h2><p>Start at zero, as for Mandelbrot. Before squaring, make both components of the current number nonnegative. This fold creates jagged structures that resemble ships and flames.</p><p>For the familiar upright ship, this viewer draws the imaginary axis increasing downward. The other fractals use an upward imaginary axis.</p>",
                              "<h2>Před umocněním přeložte</h2><p>Začněte nulou jako u Mandelbrotovy množiny. Před umocněním nahraďte obě složky aktuálního čísla jejich absolutními hodnotami. Toto přeložení vytváří ostré tvary připomínající lodě a plameny.</p><p>Aby loď měla obvyklou polohu, imaginární osa zde roste směrem dolů. U ostatních fraktálů roste nahoru.</p>"); break;
        case Fractal::Tricorn:
            body += choose(cs,"<h2>A reflection changes everything</h2><p>Before squaring, reflect the current number across the real axis: change the sign of its imaginary part. Then add c and repeat from zero.</p><p>This small change produces three prominent arms. Compare the result with Mandelbrot: similar ingredients need not produce the same geometry.</p>",
                              "<h2>Zrcadlení všechno změní</h2><p>Před umocněním zrcadlete aktuální číslo podle reálné osy: změňte znaménko imaginární části. Pak přičtěte c a opakujte od nuly.</p><p>Tato malá změna vytváří tři výrazná ramena. Porovnejte výsledek s Mandelbrotovou množinou: podobné přísady nemusí vést ke stejné geometrii.</p>"); break;
        }
        body += choose(cs,"<h2>Reading the colors</h2><p>Colors show how quickly a path escapes, with a continuous gradient between iteration counts. Dark pixels have not escaped within the selected limit; that finite test is not proof that they stay bounded forever.</p>",
                          "<h2>Jak číst barvy</h2><p>Barvy ukazují rychlost úniku dráhy a plynule přecházejí mezi počty iterací. Tmavé pixely do zvoleného limitu neunikly; konečný výpočet nedokazuje, že zůstanou omezené navždy.</p>");
    } else if (tab == 1) {
        body = "<h1>" + choose(cs,"Inside the iteration","Uvnitř iterace") + "</h1>";
        body += choose(cs,"<p>Write z = x + iy and c = a + ib, where i² = −1. An iteration applies the same function again; the sequence z₀, z₁, … is an <b>orbit</b>.</p>",
                          "<p>Pišme z = x + iy a c = a + ib, kde i² = −1. Iterace znamená další použití stejné funkce; posloupnost z₀, z₁, … tvoří <b>dráhu</b>.</p>");
        switch (kind) {
        case Fractal::Mandelbrot:
            body += choose(cs,"<h2>Vary the parameter</h2><p class='formula'>z₀ = 0<br>zₙ₊₁ = zₙ² + c</p><p>The Mandelbrot set consists of the parameters c for which this orbit is bounded. In real coordinates:</p>",
                              "<h2>Měníme parametr</h2><p class='formula'>z₀ = 0<br>zₙ₊₁ = zₙ² + c</p><p>Mandelbrotovu množinu tvoří parametry c, pro které je tato dráha omezená. V reálných souřadnicích:</p>");
            body += "<p class='formula'>x′ = x² − y² + a<br>y′ = 2xy + b</p>"; break;
        case Fractal::Julia:
            body += choose(cs,"<h2>Vary the initial value</h2><p class='formula'>z₀ = pixel<br>zₙ₊₁ = zₙ² + c</p><p>Here c is fixed. The filled set K(c) contains starting points with bounded orbits; the Julia set J(c) is its boundary. For c = 0, K(c) is the closed unit disk and J(c) is the unit circle.</p><p>For quadratic maps, K(c) is connected exactly when c belongs to the Mandelbrot set. This links the parameter map to the individual dynamical pictures.</p>",
                              "<h2>Měníme počáteční hodnotu</h2><p class='formula'>z₀ = pixel<br>zₙ₊₁ = zₙ² + c</p><p>Zde je c pevné. Vyplněná množina K(c) obsahuje počáteční body s omezenými drahami; Juliova množina J(c) je její hranicí. Pro c = 0 je K(c) uzavřený jednotkový kruh a J(c) jednotková kružnice.</p><p>U kvadratických zobrazení je K(c) souvislá právě tehdy, když c patří do Mandelbrotovy množiny. Mapa parametrů tak souvisí s jednotlivými dynamickými obrazy.</p>"); break;
        case Fractal::BurningShip:
            body += choose(cs,"<h2>Absolute values introduce a fold</h2><p class='formula'>z₀ = 0<br>x′ = x² − y² + a<br>y′ = 2|x||y| + b</p><p>Both new components use the old x and y. The absolute values break complex analyticity, so this is best treated as a map of two real variables. As with Mandelbrot, each pixel is a parameter c and the orbit begins at zero.</p>",
                              "<h2>Absolutní hodnoty překládají rovinu</h2><p class='formula'>z₀ = 0<br>x′ = x² − y² + a<br>y′ = 2|x||y| + b</p><p>Obě nové složky používají původní x a y. Absolutní hodnoty narušují komplexní analytičnost, proto je vhodné pracovat se zobrazením dvou reálných proměnných. Každý pixel je parametr c a dráha začíná v nule.</p>"); break;
        case Fractal::Tricorn:
            body += choose(cs,"<h2>Complex conjugation</h2><p class='formula'>z₀ = 0<br>zₙ₊₁ = z̄ₙ² + c<br>x′ = x² − y² + a<br>y′ = −2xy + b</p><p>The bar means conjugation: x + iy becomes x − iy. The map is antiholomorphic. The parameter set has threefold rotational symmetry, unlike Mandelbrot’s familiar two-sided reflection symmetry.</p>",
                              "<h2>Komplexní sdružení</h2><p class='formula'>z₀ = 0<br>zₙ₊₁ = z̄ₙ² + c<br>x′ = x² − y² + a<br>y′ = −2xy + b</p><p>Pruh značí komplexní sdružení: x + iy se změní na x − iy. Zobrazení je antiholomorfní. Množina parametrů má trojčetnou rotační symetrii, zatímco u Mandelbrotovy množiny snadno vidíme zrcadlovou symetrii.</p>"); break;
        }
        body += choose(cs,"<h2>What the computer tests</h2><p>The modulus |z| = √(x² + y²) measures distance from zero. We stop when |z| exceeds R = max(4, |c| + 1), a conservative escape radius for these maps, or when the iteration limit is reached.</p><p>Escaped pixels use a fractional count μ = n + 1 − log₂(log |zₙ|) to interpolate the palette. More iterations improve the boundary approximation but require more calculation; zooming does not remove that uncertainty.</p>",
                          "<h2>Co počítač ověřuje</h2><p>Modul |z| = √(x² + y²) měří vzdálenost od nuly. Končíme, když |z| překročí R = max(4, |c| + 1), bezpečný únikový poloměr pro tato zobrazení, nebo když dosáhneme limitu iterací.</p><p>Uniklé pixely používají neceločíselný počet μ = n + 1 − log₂(log |zₙ|) k interpolaci barev. Více iterací zpřesňuje přiblížení hranice, ale vyžaduje více výpočtů; samotné zvětšení tuto nejistotu neodstraní.</p>");
    } else {
        body = "<h1>" + choose(cs,"An idea with a history","Myšlenka s historií") + "</h1>";
        switch (kind) {
        case Fractal::Mandelbrot:
            body += choose(cs,"<h2>1980 · Seeing parameter space</h2><p>Benoît Mandelbrot’s 1980 paper explored fractal aspects of iterating complex quadratic maps. Computer pictures helped make their intricate parameter space visible.</p><p>The set carries his name. Its importance is not just its appearance: it organizes how an entire family of quadratic dynamical systems behaves.</p>",
                              "<h2>1980 · Pohled do prostoru parametrů</h2><p>Článek Benoîta Mandelbrota z roku 1980 zkoumal fraktální vlastnosti iterací komplexních kvadratických zobrazení. Počítačové obrazy pomohly zviditelnit jejich složitý prostor parametrů.</p><p>Množina nese jeho jméno. Není významná jen svým vzhledem: uspořádává chování celé rodiny kvadratických dynamických systémů.</p>"); break;
        case Fractal::Julia:
            body += choose(cs,"<h2>1918 · Dynamics before desktop computers</h2><p>Gaston Julia published his substantial memoir on iterating rational functions in 1918. These questions were studied mathematically long before interactive computer images became possible.</p><p>The name honors Julia. Today you can vary a parameter and inspect an orbit in seconds, connecting an abstract definition to a visible landscape.</p>",
                              "<h2>1918 · Dynamika před osobními počítači</h2><p>Gaston Julia vydal v roce 1918 rozsáhlé pojednání o iteracích racionálních funkcí. Tyto otázky se matematicky zkoumaly dávno před možností interaktivního počítačového zobrazení.</p><p>Název připomíná Juliovu práci. Dnes můžete během okamžiku změnit parametr a prozkoumat dráhu, a propojit tak abstraktní definici s viditelnou krajinou.</p>"); break;
        case Fractal::BurningShip:
            body += choose(cs,"<h2>1992 · A different kind of voyage</h2><p>Michael Michelitsch and Otto E. Rössler introduced the Burning Ship in their 1992 paper, <i>The burning ship and its quasi-Julia sets</i>.</p><p>Its absolute-value modification shows how a small change to an iteration can create a distinctive geometry. The ship-like appearance also depends on the chosen plotting orientation.</p>",
                              "<h2>1992 · Jiný druh plavby</h2><p>Michael Michelitsch a Otto E. Rössler představili Hořící loď v článku <i>The burning ship and its quasi-Julia sets</i> z roku 1992.</p><p>Úprava pomocí absolutních hodnot ukazuje, jak malá změna iterace vytváří osobitou geometrii. Podoba lodi závisí také na zvolené orientaci zobrazení.</p>"); break;
        case Fractal::Tricorn:
            body += choose(cs,"<h2>A reflected relative</h2><p>John Milnor’s paper <i>Remarks on Iterated Cubic Maps</i>, published in 1992, describes tricorn-shaped configurations in cubic parameter spaces and a model with exact threefold symmetry. The quadratic family z̄² + c gives the tricorn explored here.</p><p>Also called the Mandelbar set, it reminds us that adding a reflection changes more than the picture: the underlying dynamics differ too.</p>",
                              "<h2>Zrcadlený příbuzný</h2><p>Článek Johna Milnora <i>Remarks on Iterated Cubic Maps</i>, vydaný v roce 1992, popisuje trojrohé konfigurace v prostorech parametrů kubických zobrazení a model s přesnou trojčetnou symetrií. Kvadratická rodina z̄² + c vytváří zde zkoumaný trojrožec.</p><p>Říká se mu také Mandelbarova množina. Připomíná, že přidané zrcadlení nemění jen obraz, ale také samotnou dynamiku.</p>"); break;
        }
    }
    return "<html><head><style>body {color:#d8e3e8; font-size:14px;} h1 {font-size:24px; color:#f3f5ed; font-weight:600;} h2 {font-size:17px; color:#86d4c4; margin-top:24px;} h3 {font-size:14px; color:#86d4c4;} p {line-height:145%;} .formula {font-family:monospace; color:#efc58d; margin:18px 0;} a {color:#86d4c4;} .note {color:#91a5b3; font-size:12px;} hr {color:#2b3c4d;}</style></head><body>"
        + body + sources(kind, cs) + "</body></html>";
}
