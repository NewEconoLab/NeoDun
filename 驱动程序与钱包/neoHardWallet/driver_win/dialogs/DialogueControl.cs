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
        private static List<Window> list = new List<Window>();

        public static void CloseAllWindow()
        {
            foreach (var w in list)
                w.Close();
        }


        public static void ShowMessageDialogue(string result, int time, Window window)
        {
            if (!window.IsActive)
                return;
            MessageDialogue messageDialogue = new MessageDialogue(result);
            messageDialogue.WindowStartupLocation = WindowStartupLocation.CenterOwner;
            messageDialogue.Owner = window;
            messageDialogue.Show(time);
        }


        public static void ShowImportWalletDialogue(DriverControl driverControl,Window window)
        {
            if (!window.IsActive)
                return;
            ImportWalletDialogue importWalletDialogue = new ImportWalletDialogue(driverControl);
            list.Add(importWalletDialogue);
            importWalletDialogue.WindowStartupLocation = WindowStartupLocation.CenterOwner;
            importWalletDialogue.Owner = window;
            importWalletDialogue.ShowDialog();
            list.Remove(importWalletDialogue);
        }

        public static void ShowImportWifDialogue(DriverControl driverControl, Window window)
        {
            if (!window.IsActive)
                return;
            ImportWifDialogue importWifDialogue = new ImportWifDialogue(driverControl);
            list.Add(importWifDialogue);
            importWifDialogue.WindowStartupLocation = WindowStartupLocation.CenterOwner;
            importWifDialogue.Owner = window;
            importWifDialogue.ShowDialog();
            list.Remove(importWifDialogue);
        }

        public static void ShowAddressListDialogue(DriverControl driverControl, Window window)
        {
            if (!window.IsActive)
                return;
            AddressListDialogue addressListDialogue = new AddressListDialogue(driverControl);
            list.Add(addressListDialogue);
            addressListDialogue.WindowStartupLocation = WindowStartupLocation.CenterOwner;
            addressListDialogue.Owner = window;
            addressListDialogue.ShowDialog();
            list.Remove(addressListDialogue);
        }

        public static void ShowImportAddressListDialogue(ThinNeo.NEP6.NEP6Wallet nep6wallet,string password, DriverControl driverControl, Window window)
        {
            if (!window.IsActive)
                return;
            ImportAddressListDialogue importAddressListDialogue = new ImportAddressListDialogue(driverControl);
            list.Add(importAddressListDialogue);
            importAddressListDialogue.WindowStartupLocation = WindowStartupLocation.CenterOwner;
            importAddressListDialogue.Owner = window;
            importAddressListDialogue.ShowAddressInfo(nep6wallet, password);
            importAddressListDialogue.ShowDialog();
            list.Remove(importAddressListDialogue);
        }
    }
}
