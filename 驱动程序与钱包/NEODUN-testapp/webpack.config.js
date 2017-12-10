module.exports = {
    entry: './js/index.js',
    output: {
        path: __dirname,
        filename: './js/dist/bundle.js'
    },
    module: {
        loaders: [{
            test: /\.js$/,
            exclude: /(node_modules|bower_components)/,
            use: {
                loader: 'babel-loader',
                options: {
                    presets: ['env'],
                    plugins: [require('babel-plugin-transform-object-rest-spread')]
                }
            }
        }]
    },
    node: {
        fs: 'empty'
    }
}