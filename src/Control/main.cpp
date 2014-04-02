#include "stdafx.h"
#include "Propello.h"

// create a generic Qt application
int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    app.setOrganizationName("Ultrasonix");
    app.setApplicationName("Propello");

    QResource::registerResource("propello.rcc");

    Propello propello;
    propello.setWindowTitle("Ultrasteer by Stanford CHARM Lab");
    propello.show();
	
    return app.exec();
}
