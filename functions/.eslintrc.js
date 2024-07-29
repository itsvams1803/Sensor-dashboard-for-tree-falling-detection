module.exports = {
  env: {
    browser: true,
    es2021: true,
    node: true,
  },
  extends: ['react-app'],
  parserOptions: {
    ecmaFeatures: {
      jsx: true,
    },
    ecmaVersion: 12,
    sourceType: 'module',
  },
  settings: {
    react: {
      version: 'detect',
    },
  },
  rules: {
    'max-len': ['error', { code: 100 }],
    // Add more rules as needed
  },
};

