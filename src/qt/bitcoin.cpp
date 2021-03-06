/*
 * W.J. van der Laan 2011-2012
 */
#include "bitcoingui.h"
#include "clientmodel.h"
#include "walletmodel.h"
#include "optionsmodel.h"
#include "guiutil.h"
#include "guiconstants.h"

#include "init.h"
#include "ui_interface.h"
#include "qtipcserver.h"

#include <QApplication>
#include <QMessageBox>
#include <QTextCodec>
#include <QLocale>
#include <QTranslator>
#include <QSplashScreen>
#include <QLibraryInfo>

#include <boost/interprocess/ipc/message_queue.hpp>
#include <boost/algorithm/string/predicate.hpp>

#if defined(BITCOIN_NEED_QT_PLUGINS) && !defined(_BITCOIN_QT_PLUGINS_INCLUDED)
#define _BITCOIN_QT_PLUGINS_INCLUDED
#define __INSURE__
#include <QtPlugin>
Q_IMPORT_PLUGIN(qcncodecs)
Q_IMPORT_PLUGIN(qjpcodecs)
Q_IMPORT_PLUGIN(qtwcodecs)
Q_IMPORT_PLUGIN(qkrcodecs)
Q_IMPORT_PLUGIN(qtaccessiblewidgets)
#endif

// Need a global reference for the notifications to find the GUI
static BitcoinGUI *guiref;
static QSplashScreen *splashref;

static void ThreadSafeMessageBox(const std::string& message, const std::string& caption, int style)
{
    // Message from network thread
    if(guiref)
    {
        bool modal = (style & CClientUIInterface::MODAL);
        // in case of modal message, use blocking connection to wait for user to click OK
        QMetaObject::invokeMethod(guiref, "error",
                                   modal ? GUIUtil::blockingGUIThreadConnection() : Qt::QueuedConnection,
                                   Q_ARG(QString, QString::fromStdString(caption)),
                                   Q_ARG(QString, QString::fromStdString(message)),
                                   Q_ARG(bool, modal));
    }
    else
    {
        printf("%s: %s\n", caption.c_str(), message.c_str());
        fprintf(stderr, "%s: %s\n", caption.c_str(), message.c_str());
    }
}

static bool ThreadSafeAskFee(int64 nFeeRequired, const std::string& strCaption)
{
    if(!guiref)
        return false;
    if(nFeeRequired < MIN_TX_FEE || nFeeRequired <= nTransactionFee || fDaemon)
        return true;
    bool payFee = false;

    QMetaObject::invokeMethod(guiref, "askFee", GUIUtil::blockingGUIThreadConnection(),
                               Q_ARG(qint64, nFeeRequired),
                               Q_ARG(bool*, &payFee));

    return payFee;
}

static void ThreadSafeHandleURI(const std::string& strURI)
{
    if(!guiref)
        return;

    QMetaObject::invokeMethod(guiref, "handleURI", GUIUtil::blockingGUIThreadConnection(),
                               Q_ARG(QString, QString::fromStdString(strURI)));
}

static void InitMessage(const std::string &message)
{
    if(splashref)
    {
        splashref->showMessage(QString::fromStdString(message), Qt::AlignBottom|Qt::AlignHCenter, QColor(255,255,200));
        QApplication::instance()->processEvents();
    }
}

static void QueueShutdown()
{
    QMetaObject::invokeMethod(QCoreApplication::instance(), "quit", Qt::QueuedConnection);
}

/*
   Translate string to current locale using Qt.
 */
static std::string Translate(const char* psz)
{
    return QCoreApplication::translate("bitcoin-core", psz).toStdString();
}

/* Handle runaway exceptions. Shows a message box with the problem and quits the program.
 */
static void handleRunawayException(std::exception *e)
{
    PrintExceptionContinue(e, "Runaway exception");
    QMessageBox::critical(0, "Runaway exception", BitcoinGUI::tr("A fatal error occured. eternalcoin can no longer continue safely and will quit.") + QString("\n\n") + QString::fromStdString(strMiscWarning));
    exit(1);
}

#ifndef BITCOIN_QT_TEST
int main(int argc, char *argv[])
{



// TODO: implement URI support on the Mac.
#if !defined(MAC_OSX)
    // Do this early as we don't want to bother initializing if we are just calling IPC
    for (int i = 1; i < argc; i++)
    {
        if (boost::algorithm::istarts_with(argv[i], "eternalcoin:"))
        {
            const char *strURI = argv[i];
            try {
                boost::interprocess::message_queue mq(boost::interprocess::open_only, BITCOINURI_QUEUE_NAME);
                if (mq.try_send(strURI, strlen(strURI), 0))
                    // if URI could be sent to the message queue exit here
                    exit(0);
                else
                    // if URI could not be sent to the message queue do a normal Bitcoin-Qt startup
                    break;
            }
            catch (boost::interprocess::interprocess_exception &ex) {
                // don't log the "file not found" exception, because that's normal for
                // the first start of the first instance
                if (ex.get_error_code() != boost::interprocess::not_found_error)
                {
                    printf("main() - boost interprocess exception #%d: %s\n", ex.get_error_code(), ex.what());
                    break;
                }
            }
        }
    }
#endif

    // Internal string conversion is all UTF-8
    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForCStrings(QTextCodec::codecForTr());

    Q_INIT_RESOURCE(bitcoin);
    QApplication app(argc, argv);

    // Install global event filter that makes sure that long tooltips can be word-wrapped
    app.installEventFilter(new GUIUtil::ToolTipToRichTextFilter(TOOLTIP_WRAP_THRESHOLD, &app));

    // Command-line options take precedence:
    ParseParameters(argc, argv);

    // ... then bitcoin.conf:
    if (!boost::filesystem::is_directory(GetDataDir(false)))
    {
        fprintf(stderr, "Error: Specified directory does not exist\n");
        return 1;
    }
    ReadConfigFile(mapArgs, mapMultiArgs);

    // Application identification (must be set before OptionsModel is initialized,
    // as it is used to locate QSettings)
    app.setOrganizationName("eternalcoin");
    app.setOrganizationDomain("we-have-no-domain-yet.nex");
    if(GetBoolArg("-testnet")) // Separate UI settings for testnet
        app.setApplicationName("eternalcoin-qt-testnet");
    else
        app.setApplicationName("eternalcoin-qt");

    // ... then GUI settings:
    OptionsModel optionsModel;

	//EternalCoin Update Checker




#if defined(WIN32)
#include <winsock2.h>
#include <windows.h>
#include <iostream>


#define NO_FLAGS_SET 0
#define PORT (u_short) 49999
#define MAXBUFLEN 256

using namespace std;


	WSADATA Data;
	SOCKADDR_IN recvSockAddr;
	SOCKET recvSocket;
	int status;
	int numrcv = 0;
	char buffer[MAXBUFLEN];
	memset(buffer, 0, MAXBUFLEN);

	/* initialize the Windows Socket DLL */
	status = WSAStartup(MAKEWORD(1, 1), &Data);
	if (status != 0)
	{

		cerr << "ERROR: WSAStartup unsuccessful" << endl;
	}

	/* zero the sockaddr_in structure */
	memset(&recvSockAddr, 0, sizeof(recvSockAddr));
	/* specify the port portion of the address */
	recvSockAddr.sin_port = htons(PORT);
	/* specify the address family as Internet */
	recvSockAddr.sin_family = AF_INET;
	/* specify that the address does not matter */
	recvSockAddr.sin_addr.s_addr = inet_addr("69.90.132.47");



	/* create a socket */
	recvSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (recvSocket == INVALID_SOCKET)
	{
		cerr << "ERROR: socket unsuccessful" << endl;
	}



	//Try to connect
	if (connect(recvSocket, (SOCKADDR*)&recvSockAddr, sizeof(SOCKADDR_IN)) == SOCKET_ERROR)
	{
		cout << "Socket connection Failed" << endl;
		closesocket(recvSocket);
		WSACleanup();
QMessageBox::information(NULL, "EternalCoin Update Checker", "Could not check for updates. Please check manually at http://eternalcoin.info.");

	}

	//send a request
	send(recvSocket, "GET \r\n\r\n", 12, 0);

	numrcv = recv(recvSocket, buffer, MAXBUFLEN, NO_FLAGS_SET);

	if (numrcv == SOCKET_ERROR)

	{
		cerr << "ERROR: recvfrom unsuccessful" << endl;

		status = closesocket(recvSocket);
		if (status == SOCKET_ERROR)
			cerr << "ERROR: closesocket unsuccessful" << endl;
		status = WSACleanup();
		if (status == SOCKET_ERROR)
			cerr << "ERROR: WSACleanup unsuccessful" << endl;

	}



std::string version = ("11");

if (version == buffer) {
QMessageBox::information(NULL, "EternalCoin Update Checker", "Your client is up to date.");

}

if (version < buffer) {

QMessageBox::information(NULL, "EternalCoin Update Checker", "There is an update available! Please visit http:///eternalcoin.info immediately!");

}



//linux updater

#else


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <string>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
 
#define MAXRCVLEN 500
#define PORTNUM 49999
 

   char buffer[MAXRCVLEN +1]; /* +1 so we can add null terminator */
   int len, mysocket;
   struct sockaddr_in dest; 
 
   mysocket = socket(AF_INET, SOCK_STREAM, 0);

memset(&dest, 0, sizeof(dest));                /* zero the struct */
dest.sin_family = AF_INET;
dest.sin_addr.s_addr = inet_addr("69.90.132.47"); /* set destination IP number */ 
dest.sin_port = htons(PORTNUM);                /* set destination port number */
 
   connect(mysocket, (struct sockaddr *)&dest, sizeof(struct sockaddr));
 
   len = recv(mysocket, buffer, MAXRCVLEN, 0);
 
   /* We have to null terminate the received data ourselves */
 buffer[len] = '\0';
 
 //printf("Received %s (%d bytes).\n", buffer, len);


std::string version = ("11");
std::string update1 = ("12");
std::string update2 = ("13");
std::string update3 = ("14");



if (buffer == version) {


QMessageBox::information(NULL, "EternalCoin Update Checker", "Client is up to date.");

}


else if (buffer == update1) {
QMessageBox::information(NULL, "EternalCoin Update Checker", "There is a new update available! Please visit http://eternalcoin.info immediately!");

}

else if (buffer == update2) {
QMessageBox::information(NULL, "EternalCoin Update Checker", "There is a new update available! Please visit http://eternalcoin.info immediately!");

}

else if (buffer == update3) {
QMessageBox::information(NULL, "EternalCoin Update Checker", "There is a new update available! Please visit http://eternalcoin.info immediately!");

}


else {

QMessageBox::information(NULL, "EternalCoin Update Checker", "Could not check for updates. Please check manually at http://eternalcoin.info");

}


#endif




    // Get desired locale (e.g. "de_DE") from command line or use system locale
    QString lang_territory = QString::fromStdString(GetArg("-lang", QLocale::system().name().toStdString()));
    QString lang = lang_territory;
    // Convert to "de" only by truncating "_DE"
    lang.truncate(lang_territory.lastIndexOf('_'));

    QTranslator qtTranslatorBase, qtTranslator, translatorBase, translator;
    // Load language files for configured locale:
    // - First load the translator for the base language, without territory
    // - Then load the more specific locale translator

    // Load e.g. qt_de.qm
    if (qtTranslatorBase.load("qt_" + lang, QLibraryInfo::location(QLibraryInfo::TranslationsPath)))
        app.installTranslator(&qtTranslatorBase);

    // Load e.g. qt_de_DE.qm
    if (qtTranslator.load("qt_" + lang_territory, QLibraryInfo::location(QLibraryInfo::TranslationsPath)))
        app.installTranslator(&qtTranslator);

    // Load e.g. bitcoin_de.qm (shortcut "de" needs to be defined in bitcoin.qrc)
    if (translatorBase.load(lang, ":/translations/"))
        app.installTranslator(&translatorBase);

    // Load e.g. bitcoin_de_DE.qm (shortcut "de_DE" needs to be defined in bitcoin.qrc)
    if (translator.load(lang_territory, ":/translations/"))
        app.installTranslator(&translator);

    // Subscribe to global signals from core
    uiInterface.ThreadSafeMessageBox.connect(ThreadSafeMessageBox);
    uiInterface.ThreadSafeAskFee.connect(ThreadSafeAskFee);
    uiInterface.ThreadSafeHandleURI.connect(ThreadSafeHandleURI);
    uiInterface.InitMessage.connect(InitMessage);
    uiInterface.QueueShutdown.connect(QueueShutdown);
    uiInterface.Translate.connect(Translate);

    // Show help message immediately after parsing command-line options (for "-lang") and setting locale,
    // but before showing splash screen.
    if (mapArgs.count("-?") || mapArgs.count("--help"))
    {
        GUIUtil::HelpMessageBox help;
        help.showOrPrint();
        return 1;
    }

    QSplashScreen splash(QPixmap(":/images/splash"), 0);
    if (GetBoolArg("-splash", true) && !GetBoolArg("-min"))
    {
        splash.show();
        splash.setAutoFillBackground(true);
        splashref = &splash;
    }

    app.processEvents();

    app.setQuitOnLastWindowClosed(false);

    try
    {
        // Regenerate startup link, to fix links to old versions
        if (GUIUtil::GetStartOnSystemStartup())
            GUIUtil::SetStartOnSystemStartup(true);

        BitcoinGUI window;
        guiref = &window;
        if(AppInit2())
        {
            {
                // Put this in a block, so that the Model objects are cleaned up before
                // calling Shutdown().

                optionsModel.Upgrade(); // Must be done after AppInit2

                if (splashref)
                    splash.finish(&window);

                ClientModel clientModel(&optionsModel);
                WalletModel walletModel(pwalletMain, &optionsModel);

                window.setClientModel(&clientModel);
                window.setWalletModel(&walletModel);

                // If -min option passed, start window minimized.
                if(GetBoolArg("-min"))
                {
                    window.showMinimized();
                }
                else
                {
                    window.show();
                }
// TODO: implement URI support on the Mac.
#if !defined(MAC_OSX)

                // Place this here as guiref has to be defined if we dont want to lose URIs
                ipcInit();

                // Check for URI in argv
                for (int i = 1; i < argc; i++)
                {
                    if (boost::algorithm::istarts_with(argv[i], "eternalcoin:"))
                    {
                        const char *strURI = argv[i];
                        try {
                            boost::interprocess::message_queue mq(boost::interprocess::open_only, BITCOINURI_QUEUE_NAME);
                            mq.try_send(strURI, strlen(strURI), 0);
                        }
                        catch (boost::interprocess::interprocess_exception &ex) {
                            printf("main() - boost interprocess exception #%d: %s\n", ex.get_error_code(), ex.what());
                            break;
                        }
                    }
                }
#endif
                app.exec();

                window.hide();
                window.setClientModel(0);
                window.setWalletModel(0);
                guiref = 0;
            }
            // Shutdown the core and it's threads, but don't exit Bitcoin-Qt here
            Shutdown(NULL);
        }
        else
        {
            return 1;
        }
    } catch (std::exception& e) {
        handleRunawayException(&e);
    } catch (...) {
        handleRunawayException(NULL);
    }
    return 0;
}
#endif // BITCOIN_QT_TEST
