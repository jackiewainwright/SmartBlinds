const config = __DEV__ ? require('./development') : require('./production');

const commonConfig = require('./common');

const finalConfig = Object.assign(commonConfig.default, config.default);

export default finalConfig;
