const { DataTypes } = require('sequelize');

module.exports = (sequelize) => {
  const Users = sequelize.define('Users', {
    username: {
      type: DataTypes.STRING(16),
      allowNull: false,
      primaryKey: true,
      isUnique: true
    },
    email: {
      type: DataTypes.STRING(64),
      allowNull: true,
    },
    password: {
      type: DataTypes.STRING(64),
      allowNull: false
    },
    isAdmin: {
      type: DataTypes.INTEGER,
      default: 0
    }
  }, { timestamps: false });

  return Users;
}; 