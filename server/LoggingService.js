class LoggingService {
    constructor(operationModel) {
        this.OperationModel = operationModel;
        this.timeWindow = 20; // in seconds
    }
    async addOperation(username, operationName, operationDate) {
        try {
            await this.OperationModel.create({userUsername: username, operationName: operationName, operationDate: operationDate});
        } catch(error) {
            console.error("Error adding operation to logging system: ", error);
        }
    }
}

module.exports = { LoggingService };
