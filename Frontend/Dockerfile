FROM mcr.microsoft.com/dotnet/sdk:8.0 AS build
ARG BUILD_CONFIGURATION=Release
WORKDIR /src
COPY Frontend.csproj .
RUN dotnet restore

COPY . .
RUN dotnet publish -c $BUILD_CONFIGURATION -o /publish

ENTRYPOINT ["dotnet", "run", "--project", "Frontend.csproj", "--urls", "http://+:7088"]