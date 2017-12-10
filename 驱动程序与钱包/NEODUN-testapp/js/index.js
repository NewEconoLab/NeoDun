import chai from 'chai';
import {
    ab2str,
    str2ab,
    hexstring2ab,
    ab2hexstring,
    reverseArray,
    numStoreInMemory,
    stringToBytes
} from './src/utils';
import * as wallet from './src/wallet';
import * as api from './src/api';
import axios from 'axios';
var chaiAsPromised = require("chai-as-promised");
chai.use(chaiAsPromised);
const should = chai.should();
let isMainNet = false;

if (localStorage.wallets == undefined || localStorage.wallets == "[]" || localStorage.wallets == "") {
    localStorage.setItem("wallets", "[]");
}
/**
 * 
 * @param {*钱包名称} name 
 * @param {*地址} address 
 * @param {*余额} allassets 
 * @param {*签名数据} ciphertext 
 * @param {*是否有私钥} ifPrivate 
 */
function createShowObj(name, address, allassets, ciphertext, ifPrivate) {

    let obj = {
        name: name,
        allassets: allassets,
        showAddress: address.substr(0, 4) + "***" + address.substr(address.length - 4, address.length - 1),
        ciphertext: ciphertext,
        address: address,
        ifPrivate: ifPrivate
    };
    return obj;
};

class WalletObject {
    constructor(name, address, allassets, ciphertext, ifPrivate,ifDriver,state) {
        this.name = name;
        this.address = address;
        this.showAddress = address.substr(0, 4) + "***" + address.substr(address.length - 4, address.length - 1);
        this.allassets = allassets;
        this.ciphertext = ciphertext;
        this.ifPrivate = ifPrivate;
        this.ifDriver = ifDriver;
        this.state = state
    };

}

/**
 * 钱包工具类
 */
class WalletList {
    constructor() {
        this.address = "";
        this.currency = "";
        this.number = 0;
    };
    /**
     * 添加钱包
     * @param {*钱包对象} walletObj 
     */
    addWallet(walletObj) {
        let wallets = localStorage.wallets;
        // this.address = address;
        if (!this.findAddress(walletObj.address)) {
            let walletArr = Array();
            walletArr = JSON.parse(localStorage.wallets);
            walletArr.push(walletObj);
            localStorage.wallets = JSON.stringify(walletArr);
            return true;
        }
        return false;
    };
    /**
     * 删除钱包
     * @param {*钱包数组下标} index 
     */
    deleteWallet(index) {
        let wallets = Array();
        wallets = JSON.parse(localStorage.wallets);
        wallets.splice(index, 1);
        localStorage.wallets = JSON.stringify(wallets);
        return true;
    };
    //修改钱包对象
    setWallet($index, $obj) {

        let wallets = Array();
        wallets = JSON.parse(localStorage.wallets);
        wallets[$index] = $obj;
        localStorage.wallets = JSON.stringify(wallets);
        return true;
    };
    /**
     * 获得未使用资产
     * @param {*地址} address 
     */
    getUnspent($address, $isMainNet) {
        let unspent = new Array();
        console.log($isMainNet);
        api.getBalance($isMainNet ? api.MAINNET : api.TESTNET, $address).then((response) => {
            unspent.push({ name: '小蚁股', balance: response.ANS });
            unspent.push({ name: '小蚁币', balance: response.ANC });
        });
        return unspent;
    };
    /**
     * 获得钱包
     */
    getWallets(isMainNet) {
        let wallets = localStorage.wallets;
        let showWallets = new Array();
        if (wallets != undefined && wallets != '') {
            let arr = JSON.parse(wallets);
            if (arr.length != 0) {
                let usarr = new Array();
                for (var i = 0; i < arr.length; i++) {
                    let address = arr[i].address;
                    let name = arr[i].name;
                    let ciphertext = arr[i].ciphertext;
                    let allassets = this.getUnspent(address, isMainNet);
                    let show = new WalletObject(name, address, allassets, ciphertext, ciphertext != undefined && ciphertext != '' ? true : false,ifDriver=false,state=false);
                    // let show = createShowObj(name, address, allassets, ciphertext, ciphertext != undefined && ciphertext != '' ? true : false);
                    showWallets.push(show);

                }
            }
        }
        return showWallets;
    };
    /**
     * 查找地址是否已存在
     * @param {*地址} $address 
     */
    findAddress($address) {
        let wallets = localStorage.wallets;
        // this.address = address;
        if (wallets != undefined) {
            let walletArr = Array();
            walletArr = JSON.parse(localStorage.wallets);
            for (var i = 0; i < walletArr.length; i++) {
                if (walletArr[i]["address"] == $address) {
                    // this.alertMessage = "校验失败，请输入正确的WIF";
                    // $('#alert').modal('show');
                    return true;
                }
            }
        } else {
            return false;
        }
    };
}
let walletList = new WalletList();
/**
 * vue模型层
 */
var vm = new Vue({
    el: '#app',
    data: {
        page: 0,
        newWallet: {
            id: '',
            name: '',
            allassets: [{ assetid: '', name: '小蚁股', list: [], balance: 0 }, { assetid: '', name: '小蚁币', list: [], balance: 0 }],
            address: '',
            currency: '',
            number: 0,
            ciphertext: '',
            password: '',
            password1: ''
        },
        setWallet: {
            name: '',
            index: 0
        },
        wallets: walletList.getWallets(isMainNet),
        loginData: {
            walletIndex: 0,
            ciphertext: '',
            password: ''
        },
        alertMessage: "",
        businessMessage: {
            toAddress: '',
            address: '',
            amount: 0,
            publickeyEncoded: '',
            privatekey: '',
            account: {},
            isNewAddress: 0
        },
        showAddress: 'false',
        showManipulate: 'false',
        deleteData: {
            index: 0,
            address: ''
        },
        reminderControl: {
            isNewAddress: { isInputErr: false, message: '' },
            isNewPassword: {
                isInput2Err: false,
                isInput1Err: false,
                message1: '',
                message2: ''
            },
            isBusiness: {
                isToAddressErr: false,
                message1: '',
                isAmountErr: false,
                message2: ''
            }
        },
        isMainNet: false,
        driverState:false,
        newDriverAddress: new Array()
    },
    mounted() {
        // console.log("页面加载完成");
        this.refreshBalance();
        this.findDriver();
    },
    methods: {
        //刷新余额
        refreshBalance() {
            setInterval(() => {
                this.wallets = walletList.getWallets(this.isMainNet);
            }, 30000);
        },
        //切换节点
        changeNode($isMainNet) {
            this.isMainNet = $isMainNet;
            isMainNet = $isMainNet;
            let nodeName = $isMainNet ? '主网' : '测试';
            this.alertMessage = '已切换' + nodeName + '网络节点';
            this.wallets = walletList.getWallets(this.isMainNet);
            $('#alert').modal('show');
        },
        //展示操作
        showAction($index) {
            var index = "" + $index;
            if (this.showManipulate == index)
                this.showManipulate = 'false';
            else
                this.showManipulate = index;
        },
        //展开或缩小地址
        addressModel($index) {
            // var index = $index;
            // this.showAddress = index;
            this.alertMessage = this.wallets[$index].address;
            $('#alert').modal('show');
        },
        //新建增钱包
        createWallet() {
            let walletObj = {
                name: '',
                allassets: new Array(),
                showAddress: '',
                address: '',
                ciphertext: '',
                ifPrivate: false
            };
            //判断是否有密码，如果有则是wif,没有则是Address
            let ret = this.verifyAddReminder('new-address');
            //添加地址
            if (ret == 'address') {
                walletObj.address = this.newWallet.address;
                walletObj.allassets = walletList.getUnspent(this.newWallet.address, this.isMainNet);
                if (walletList.addWallet(walletObj)) {
                    // let objShow = createShowObj('', walletObj.address, walletObj.allassets, '', false);
                    let objShow = new WalletObject('', walletObj.address, walletObj.allassets, '', false,ifDriver=false,state=false);
                    this.wallets.push(objShow);
                    this.alertMessage = "地址导入成功";
                    $('#createWallet').modal('hide');
                    $('#alert').modal('show');
                };
                return;
            };
            //添加wif
            if (ret == 'wif') {
                if (this.verifyAddReminder('new-password') == 'validated') {
                    let privateKey = wallet.getPrivateKeyFromWIF(this.newWallet.address);
                    let password = this.newWallet.password;
                    let encryptData = wallet.encryptNeodunPrivateKey(privateKey, password);
                    let address = wallet.getAccountsFromPrivateKey(privateKey)[0].address;
                    let allassets = walletList.getUnspent(address, this.isMainNet);
                    walletObj.address = address;
                    walletObj.ciphertext = encryptData;
                    if (walletList.addWallet(walletObj)) {
                        // let objShow = createShowObj('', address, allassets, encryptData, true);
                        let objShow = new WalletObject('', address, allassets, encryptData, true,ifDriver=false,state=false);
                        this.wallets.push(objShow);
                        this.alertMessage = "WIF导入成功";
                        $('#createPassword').modal('hide');
                        $('#alert').modal('show');
                        $('#createPassword').modal('hide');
                        return;
                    } 


                };
                return;
            };
            // 添加完newWallet对象后，重置newWallet对象
            this.newWallet = {
                id: '',
                name: '',
                allassets: [],
                address: '',
                currency: '',
                number: 0,
                ciphertext: '',
                password: '',
                password1: ''
            }
        },
        //删除钱包
        deleteWallet() {
            // 删一个数组元素
            if (walletList.deleteWallet(this.deleteData.index)) {
                this.wallets.splice(this.deleteData.index, 1);
                this.alertMessage = "删除成功";
                $('#deleteAddress').modal('hide');
                $('#alert').modal('show');
            }
        },
        //初始化昵称到
        openSetName($index) {
            this.setName = this.wallets[$index].name;
            this.setWallet.index = $index;
            $('#setName').modal('show');
        },
        //更新昵称
        setWalletName() {
            this.wallets[this.setWallet.index].name = this.setWallet.name;
            walletList.setWallet(this.setWallet.index, this.wallets[this.setWallet.index]);
            $('#setName').modal('hide');
        },
        //开启登录
        openLogin($index) {
            let index = $index;
            this.loginData.walletIndex = index;
            this.verifyClean('business');
            $('#businessOpen').modal('show');
        },
        //交易登录
        loginWallet() {
            let index = this.loginData.walletIndex;
            let ciphertext = this.wallets[index].ciphertext;
            let privateKey = wallet.decryptNeodunPrivateKey(ciphertext, this.loginData.password);
            this.businessMessage.publickeyEncoded = wallet.getPublicKey(privateKey, true);
            this.businessMessage.account = this.wallets[this.loginData.walletIndex].allassets[0];
            this.businessMessage.address = this.wallets[this.loginData.walletIndex].address;
            if (privateKey == '0') {
                // $('#loginWallet').modal('hide');
                this.alertMessage = "密码错误请重新输入";
                $('#alert').modal('show');
            } else {
                this.businessMessage.privatekey = privateKey;
                $('#loginWallet').modal('hide');
                this.alertMessage = "登录成功，开始交易吧";
                $('#businessAlert').modal('show');
            }
            //清空登录信息
            this.loginData = {
                walletIndex: index,
                ciphertext: '',
                password: ''
            }
        },
        //交易
        business() {
            if (this.verifyAddReminder('business') == 'validated') {
                $('#businessOpen').modal('hide');
                $('#loginWallet').modal('show');
            };
            return;
            // $('#businessMessage').modal('show');
        },
        //进行交易
        SignTxAndSend() {
            var publicKeyEncoded = this.businessMessage.publickeyEncoded;
            var privateKey = this.businessMessage.privatekey;
            $('#businessAlert').modal('hide');
            $('#loader').modal('show');
            // console.log(wallet.getWIFFromPrivateKey(privateKey));
            api.sendAssetTransaction(
                api.TESTNET,
                this.businessMessage.toAddress,
                wallet.getWIFFromPrivateKey(privateKey),
                'AntShares',
                this.businessMessage.amount).then((response) => {
                if (response.data.result) {
                    $('#loader').modal('hide');
                    $('#businessAlert').modal('hide');
                    this.alertMessage = "交易成功";
                    $('#alert').modal('show');
                } else {
                    $('#loader').modal('hide');
                    $('#businessAlert').modal('hide');
                    this.alertMessage = "交易失败";
                    $('#alert').modal('show');
                    console.log(response);
                }
            });
            this.businessMessage = {
                toAddress: '',
                amount: 0,
                publickeyEncoded: '',
                privatekey: '',
                account: {},
                isNewAddress: 0
            }
        },
        //验证与提醒
        verifyAddReminder($type) {
            //新增地址的验证
            if ($type == 'new-address') {
                let newAddress = this.newWallet.address;
                if (newAddress == '' || newAddress == undefined) {
                    this.reminderControl.isNewAddress.message = '地址不能为空';
                    this.reminderControl.isNewAddress.isInputErr = true;
                    return 'err-null';
                }

                if (walletList.findAddress(newAddress)) {
                    this.reminderControl.isNewAddress.message = '您已添加过此地址';
                    this.reminderControl.isNewAddress.isInputErr = true;
                    return 'err-format';
                };
                if (newAddress.length == 34) {
                    if (!wallet.verifyAddress(newAddress)) {
                        this.reminderControl.isNewAddress.message = '请输入正确格式的地址';
                        this.reminderControl.isNewAddress.isInputErr = true;
                        return 'err-format';
                    } else {
                        this.reminderControl.isNewAddress.isInputErr = false;
                        return 'address';
                    }
                } else if (newAddress.length == 52) {
                    let privateKey = wallet.getPrivateKeyFromWIF(newAddress);
                    if (privateKey == -1 || privateKey == -2) {
                        this.reminderControl.isNewAddress.message = '请输入正确的WIF';
                        this.reminderControl.isNewAddress.isInputErr = true;
                    } else {
                        this.reminderControl.isNewAddress.isInputErr = false;
                        return 'wif';
                    }
                } else {
                    this.reminderControl.isNewAddress.message = '请输入正确格式的地址或者私钥';
                    this.reminderControl.isNewAddress.isInputErr = true;
                    return 'err-format';
                }
            };
            //新增密码的验证
            if ($type == 'new-password') {
                let password = this.newWallet.password;
                let password1 = this.newWallet.password1;
                if (password1.length < 6) {
                    this.reminderControl.isNewPassword.message1 = '密码长度不得小于6位';
                    this.reminderControl.isNewPassword.isInput1Err = true;
                    // return 'err-format';
                    return 'err-format';
                } else {
                    this.reminderControl.isNewPassword.isInput1Err = false;
                }
                if (password != password1) {
                    this.reminderControl.isNewPassword.message2 = '密码不一致，请重新输入';
                    this.reminderControl.isNewPassword.isInput2Err = true;
                    // return 'err-difference';
                    return 'err-format';
                } else {
                    this.reminderControl.isNewPassword.isInput2Err = false;
                    return 'validated';
                }
            };
            //交易的验证
            if ($type == 'business') {
                var busData = this.businessMessage;
                var verifyResult = 'validated';
                if (busData.toAddress == '' || busData.toAddress == undefined) {
                    this.reminderControl.isBusiness.isToAddressErr = true;
                    this.reminderControl.isBusiness.message1 = '交易地址不能为空';
                    verifyResult = 'err-null';
                    // return 'err-null';
                } else if (!wallet.verifyAddress(busData.toAddress)) {
                    this.reminderControl.isBusiness.isToAddressErr = true;
                    this.reminderControl.isBusiness.message1 = '请输入正确的交易地址';
                    verifyResult = 'err-format';
                    // return 'err-format'
                } else {
                    this.reminderControl.isBusiness.isToAddressErr = false;
                    if (!walletList.findAddress(busData.toAddress)) {
                        this.businessMessage.isNewAddress = 1;
                    }
                    // verifyResult = 'err-format';
                };
                //校验金额
                if (isNaN(busData.amount)) {
                    this.reminderControl.isBusiness.isAmountErr = true;
                    this.reminderControl.isBusiness.message2 = '请输入正确的金额';
                    verifyResult = 'err-format';
                    // return 'err-format'
                } else if (parseFloat(busData.amount) <= 0) {
                    this.reminderControl.isBusiness.isAmountErr = true;
                    this.reminderControl.isBusiness.message2 = '金额必须大于零';
                    verifyResult = 'err-format';
                } else {
                    this.reminderControl.isBusiness.isAmountErr = false;
                };
                return verifyResult;
            };

        },
        verifyClean($type) {
            if ($type == 'new-wallet') {
                this.newWallet = {
                    id: '',
                    name: '',
                    allassets: [],
                    address: '',
                    currency: '',
                    number: 0,
                    ciphertext: '',
                    password: '',
                    password1: ''
                };
                this.reminderControl.isNewAddress.message = '';
                this.reminderControl.isNewAddress.isInputErr = false;
                this.reminderControl.isNewPassword.isInput1Err = false;
            };
            if ($type == 'business') {
                this.businessMessage = {
                    toAddress: '',
                    amount: 0,
                    publickeyEncoded: '',
                    privatekey: '',
                    account: {},
                    isNewAddress: 0
                };
                this.reminderControl.isBusiness.isToAddressErr = false;
                this.reminderControl.isBusiness.isAmountErr = false;

            };
        },
        findDriver(){
            setInterval(() => {
                api.queryStateFormDriver().then((res)=>{
                    if(res.result){        
                        // this.alertMessage = "检测到有新的驱动接入";
                        // $('#alert').modal('show');
                        // console.log(JSON.stringify(res));
                        api.queryAddressesFromDriver().then((res)=>{
                            if(res.result){
                                this.driverState = true;
                                let arr = new Array();
                                for(var a=0;a<res.addresses.length;a++){
                                    var result = walletList.findAddress(res.addresses[a].address)
                                    if(!result){
                                        // this.alertMessage = "有新地址是否添加";
                                        arr.push(res.addresses[a].address);
                                        // arr.push(res.addresses[a].address)
                                    }
                                }
                                this.newDriverAddress = arr;
                                if(arr.length!=0){
                                    $('#saveDriverAddress').modal('show');
                                }
                                // console.log(walletList.findAddress(res.addresses[0].address));
                            }else{
                                this.driverState =false;
                            }
                        }).catch(function (error) {
                            　　alert(error+"没有启动驱动");
                        });
                    };
                }).catch(function (error) {
                    　　alert(error+"没有启动驱动");
                });  
            }, 30000);          
        },
        saveDriverAddress(){
            for(let a=0;a<this.newDriverAddress.length;a++){
                let allassets = walletList.getUnspent(this.newDriverAddress[a], this.isMainNet);
                let objShow = new WalletObject('', this.newDriverAddress[a], allassets, '', false,ifDriver=true,state=true);            
                this.wallets.push(objShow);
                walletList.addWallet(objShow);
            }
            
            $('#saveDriverAddress').modal('hide');

        }

    }
});