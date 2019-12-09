// module.exports = function(api) {
//   api.cache(true);
//   return {
//     presets: [
//       'babel-preset-expo'
//     ],
//     plugins: [
//       ['@babel/plugin-proposal-decorators', { legacy: true }],
//     ]
//   };
// };

module.exports = {
  presets: ['module:metro-react-native-babel-preset', 'mobx'],
  plugins: [
    ['@babel/plugin-transform-flow-strip-types'],
    ['@babel/plugin-proposal-decorators', { legacy: true }],
  ]
};
