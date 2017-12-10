// this file contains high level API functions for connecting with network resources
import qs from 'qs';
import axios from 'axios';
import { getAccountsFromWIFKey, transferTransaction, signatureData, addContract } from './wallet.js';

const apiEndpoint = "http://testnet.antchain.xyz";
const rpcEndpoint = "https://api.otcgo.cn:20332"; // testnet = 20332


// network name variables
export const MAINNET = "MainNet";
export const TESTNET = "TestNet";

// hard-code asset ids for ANS and ANC
export const ansId = "c56f33fc6ecfcd0c225c4ab356fee59390af8560be0e930faebe74a6daff7c9b";
export const ancId = "602c79718b16e442de58778e148d0b1084e3b2dffd5de6b7b16cee7969282de7";
export const allAssetIds = [ansId, ancId];

// hard-code asset names for ANS and ANC
const ansName = "小蚁股";
const ancName = "小蚁币";

// destructure explorer json responses
const ANS = '\u5c0f\u8681\u80a1';
const ANC = '\u5c0f\u8681\u5e01';
// const getAns = balance => balance.filter((val) => { return val.unit === ANS })[0];
// const getAnc = balance => balance.filter((val) => { return val.unit === ANC })[0];
const getAns = balances => balances[ansId] == undefined ? '0' : balances[ansId];
const getAnc = balances => balances[ancId] == undefined ? '0' : balances[ancId];

// return Netwok endpoints that correspond to MainNet and TestNet
export const getNetworkEndpoints = (net) => {
    if (net === MAINNET) {
        return {
            apiEndpoint: "http://antchain.xyz",
            rpcEndpoint: "https://api.otcgo.cn/mainnet"
        }
    } else {
        return {
            apiEndpoint: "http://testnet.antchain.xyz",
            rpcEndpoint: "https://api.otcgo.cn/testnet"
        }
    }
};

// wrapper for querying node RPC
// see antshares node API documentation for details
const queryRPC = (net, method, params, id = 1) => {
    const network = getNetworkEndpoints(net);
    let jsonRequest = axios.create({
        headers: { "Content-Type": "application/json" }
    });
    const jsonRpcData = { "jsonrpc": "2.0", "method": method, "params": params, "id": id };
    return jsonRequest.post(network.rpcEndpoint, jsonRpcData).then((response) => {
        return response.data;
    });
};

// get a given block by index, passing verbose (the "1" param) to have the node
// destructure metadata for us
export const getBlockByIndex = (net, block) => {
    return queryRPC(net, "getblock", [block, 1]);
}

// get the current height of the blockchain
export const getBlockCount = (net) => {
    return queryRPC(net, "getblockcount", []);
}

// use a public blockchain explorer (currently antchain.xyz) to get the current balance of an account
// returns dictionary with both ANS and ANC
export const getBalance = (net, address) => {
    const network = getNetworkEndpoints(net);
    let rpcURL = '/address/';
    let apiURL = '/api/v1/address/info/';
    return axios.get(network.rpcEndpoint + rpcURL + address)
        .then((res) => {
            if (res.data.result !== 'No Address!') {
                // get ANS
                // console.log(res.data);

                const ans = res.data.balances == undefined ? 0 : getAns(res.data.balances);
                const anc = res.data.balances == undefined ? 0 : getAnc(res.data.balances);
                return { ANS: ans, ANC: anc };
            }
        })
};

// use a public blockchain explorer (currently antchain.xyz) to get unspent transaction for an account
// TODO: rename this to getUnspentTransactions
// TODO: what we really need is an API to get all transactions. new blockchain explorer!

// export const getTransactions = (net, address, assetId) => {
//     const network = getNetworkEndpoints(net);
//     return axios.get(network.apiEndpoint + '/api/v1/address/utxo/' + address).then((response) => {
//         return response.data.utxo[assetId];
//     });
// };
export const getTransactions = (net, source, dests, amount, assetId) => {
    const network = getNetworkEndpoints(net);
    let data = { 'source': source, 'dests': dests, 'amount': amount, 'assetId': assetId };
    return axios.post(
        network.rpcEndpoint +
        '/transfer?source=' + source +
        '&dests=' + dests +
        '&amounts=' + amount +
        '&assetId=' + assetId).then((response) => {
        if (response.data['result']) {
            return response.data['transaction'];
        };
        // return response.data.utxo[assetId];
    });
};


// send ANS or ANC over the network
// "net" is "MainNet" or "TestNet"
// "toAddress" is reciever address as string
// "fromWif" is sender WIF as string
// "assetType" is "AntShares" or "AntCoins"
// "amount" is integer amount to send (or float for ANC)
export const sendAssetTransaction = (net, toAddress, fromWif, assetType, amount) => {
    alert(123123);
    const network = getNetworkEndpoints(net);
    let assetId, assetName, assetSymbol;
    if (assetType === "AntShares") {
        assetId = ansId;
        assetName = ansName;
        assetSymbol = 'ANS';
    } else if (assetType === "AntCoins") {
        assetId = ancId;
        assetName = ancName;
        assetSymbol = 'ANC';
    }
    const fromAccount = getAccountsFromWIFKey(fromWif)[0];
    return getBalance(net, fromAccount.address).then((response) => {
        const balance = response[assetSymbol];
        return getTransactions(net, fromAccount.address, toAddress, amount, assetId).then((transactions) => {
            const transaction = transactions
            //const sign = signatureData(transaction, fromAccount.privatekey);
            console.log(123123);
            const publickeyEncoded = fromAccount.publickeyEncoded;
            const jsonData = { 'publicKey': publickeyEncoded, 'signature': sign, 'transaction': transaction };
            return axios.post(network.rpcEndpoint + '/broadcast', qs.stringify(jsonData)).then((response) => {
                return response;
            }).catch((error) => {
                console.log(error);
                return error;
            });
        });
    });
};


//driver code
//find driver
export const queryStateFormDriver=()=>{
    return axios.get('http://127.0.0.1:50288/_api/state')
    .then((res)=>{
        if(res.data.count>0){
            return {result:true,count:res.data.count};
        }
    }).catch(function (error) {
        alert(error+'请求无响应');
        　　return error;
    });
}

//query address list
export const queryAddressesFromDriver=()=>{
    return axios.get('http://127.0.0.1:50288/_api/listaddress')
    .then((res)=>{
        if(res.data.addresses.length>0){
            return {result:true,addresses:res.data.addresses};
        }else{
            return{result:false};
        }
    }).catch(function (error) {
        alert(error+'网络问题');
        return{result:false};
    });
}


