const { DataTypes } = require('sequelize');

module.exports = (sequelize) => {
  const Users = sequelize.define('UserOTPVerifications', {
    username: {
      type: DataTypes.STRING(16),
      allowNull: false,
      primaryKey: true
    },
    email: {
      type: DataTypes.STRING(64),
      allowNull: false,
    },
    otp: {
      type: DataTypes.INTEGER,
      allowNull: false
    },
    createdAt: {
      type: DataTypes.DATE
    },
    expiresAt: {
      type:DataTypes.DATE
    }
  }, { timestamps: false });

  return Users;
}; 