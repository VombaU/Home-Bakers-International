const { DataTypes } = require('sequelize');

module.exports = (sequelize) => {
  const Operations = sequelize.define('Operations', {
    id: {
      type: DataTypes.INTEGER,
      autoIncrement: true,
      primaryKey: true,
      allowNull: false
    },
    operationName: {
      type: DataTypes.STRING(64),
      allowNull: false,
    },
    operationDate: {
      type: DataTypes.DATE,
      allowNull: false
    },
    userUsername: {
      type: DataTypes.STRING(16),
      allowNull: false,
    }
  }, { timestamps: false });

  return Operations;
}; 