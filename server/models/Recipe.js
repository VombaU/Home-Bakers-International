const { DataTypes } = require('sequelize');

module.exports = (sequelize) => {
  const Recipes = sequelize.define('Recipes', {
    id: {
      type: DataTypes.INTEGER,
      autoIncrement: true,
      primaryKey: true,
      allowNull: false
    },
    title: {
      type: DataTypes.STRING,
      allowNull: false
    },
    description: {
      type: DataTypes.TEXT,
      allowNull: false
    },
    thumbnailUrl: {
      type: DataTypes.STRING,
      allowNull: false
    },
    rating: {
      type: DataTypes.DECIMAL(2, 1),
      allowNull: false
    },
    cuisine: {
      type: DataTypes.STRING,
      allowNull: false
    },
    doughType: {
      type: DataTypes.STRING,
      allowNull: false
    },
    flourType: {
      type: DataTypes.STRING,
      allowNull: false
    },
    kneadingMethod: {
      type: DataTypes.STRING,
      allowNull: false
    },
    uploadDate: {
      type: DataTypes.DATE,
      allowNull: true
    },
    modifiedDate: {
      type: DataTypes.DATE,
      allowNull: true
    },
    userUsername: {
      type: DataTypes.STRING(16),
      allowNull: false
    }
  }, { timestamps: false }
);

  return Recipes;
}; 