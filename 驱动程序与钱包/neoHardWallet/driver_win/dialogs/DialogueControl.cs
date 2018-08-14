using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using driver_win.helper;

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


        public static void ShowImportWalletDialogue(Window window)
        {
            if (!window.IsActive)
                return;
            ImportWalletDialogue importWalletDialogue = new ImportWalletDialogue();
            list.Add(importWalletDialogue);
            importWalletDialogue.WindowStartupLocation = WindowStartupLocation.CenterOwner;
            importWalletDialogue.Owner = window;
            importWalletDialogue.ShowDialog();
            list.Remove(importWalletDialogue);
        }

        public static void ShowImportWifDialogue(Window window)
        {
            if (!window.IsActive)
                return;
            ImportWifDialogue importWifDialogue = new ImportWifDialogue();
            list.Add(importWifDialogue);
            importWifDialogue.WindowStartupLocation = WindowStartupLocation.CenterOwner;
            importWifDialogue.Owner = window;
            importWifDialogue.ShowDialog();
            list.Remove(importWifDialogue);
        }

        public static void ShowAddressListDialogue(Window window)
        {
            if (!window.IsActive)
                return;
            AddressListDialogue addressListDialogue = new AddressListDialogue();
            list.Add(addressListDialogue);
            addressListDialogue.WindowStartupLocation = WindowStartupLocation.CenterOwner;
            addressListDialogue.Owner = window;
            addressListDialogue.ShowDialog();
            list.Remove(addressListDialogue);
        }

        public static void ShowImportAddressListDialogue(ThinNeo.NEP6.NEP6Wallet nep6wallet,string password, Window window)
        {
            if (!window.IsActive)
                return;
            ImportAddressListDialogue importAddressListDialogue = new ImportAddressListDialogue();
            list.Add(importAddressListDialogue);
            importAddressListDialogue.WindowStartupLocation = WindowStartupLocation.CenterOwner;
            importAddressListDialogue.Owner = window;
            importAddressListDialogue.ShowAddressInfo(nep6wallet, password);
            importAddressListDialogue.ShowDialog();
            list.Remove(importAddressListDialogue);
        }


        public static void ShowVerityECDH(Window window)
        {
            if (!window.IsActive)
                return;
            VerifyECDH verifyECDH = new VerifyECDH();
            list.Add(verifyECDH);
            verifyECDH.WindowStartupLocation = WindowStartupLocation.CenterOwner;
            verifyECDH.Owner = window;
            verifyECDH.ShowDialog();
            list.Remove(verifyECDH);
        }


        public static ND_MessageBoxResult ShowMessageBox(Window window, string messageBoxText, ND_MessageBoxButton button,long waitTime = 9999999999)
        {
            if (!window.IsActive)
                return ND_MessageBoxResult.No;
            MessageBox messageBox = new MessageBox();
            list.Add(messageBox);
            messageBox.WindowStartupLocation = WindowStartupLocation.CenterOwner;
            messageBox.Owner = window;
            messageBox.Show(messageBoxText, button, waitTime);
            messageBox.ShowDialog();
            ND_MessageBoxResult result = messageBox.nD_MessageBoxResult;
            list.Remove(messageBox);
            return result;
        }
    }
}
