using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;

namespace driver_win.dialogs
{
    class DialogueControl
    {
        public static void ShowMessageDialogue(string result, int time, Window window)
        {
            MessageDialogue messageDialogue = new MessageDialogue(result);
            messageDialogue.WindowStartupLocation = WindowStartupLocation.CenterOwner;
            messageDialogue.Owner = window;
            messageDialogue.Show(time);
        }


        public static void ShowImportWalletDialogue(DriverControl driverControl,Window window)
        {
            ImportWalletDialogue importWalletDialogue = new ImportWalletDialogue(driverControl);
            importWalletDialogue.WindowStartupLocation = WindowStartupLocation.CenterOwner;
            importWalletDialogue.Owner = window;
            importWalletDialogue.ShowDialog();
        }

        public static void ShowImportWifDialogue(DriverControl driverControl, Window window)
        {
            ImportWifDialogue importWifDialogue = new ImportWifDialogue(driverControl);
            importWifDialogue.WindowStartupLocation = WindowStartupLocation.CenterOwner;
            importWifDialogue.Owner = window;
            importWifDialogue.ShowDialog();
        }

        public static void ShowAddressListDialogue(DriverControl driverControl, Window window)
        {
            AddressListDialogue addressListDialogue = new AddressListDialogue(driverControl);
            addressListDialogue.WindowStartupLocation = WindowStartupLocation.CenterOwner;
            addressListDialogue.Owner = window;
            addressListDialogue.ShowDialog();
        }
    }
}
