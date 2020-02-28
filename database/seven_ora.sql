USE [master]
GO
/****** Object:  Database [seven_ORA]    Script Date: 9/12/2019 2:30:39 PM ******/
CREATE DATABASE [seven_ORA]
 CONTAINMENT = NONE
 ON  PRIMARY 
( NAME = N'seven_ORA', FILENAME = N'C:\Program Files\Microsoft SQL Server\MSSQL14.SQLEXPRESS\MSSQL\DATA\seven_ORA.mdf' , SIZE = 8192KB , MAXSIZE = UNLIMITED, FILEGROWTH = 65536KB )
 LOG ON 
( NAME = N'seven_ORA_log', FILENAME = N'C:\Program Files\Microsoft SQL Server\MSSQL14.SQLEXPRESS\MSSQL\DATA\seven_ORA_log.ldf' , SIZE = 8192KB , MAXSIZE = 2048GB , FILEGROWTH = 65536KB )
GO
ALTER DATABASE [seven_ORA] SET COMPATIBILITY_LEVEL = 140
GO
IF (1 = FULLTEXTSERVICEPROPERTY('IsFullTextInstalled'))
begin
	EXEC [seven_ORA].[dbo].[sp_fulltext_database] @action = 'enable'
end
GO
ALTER DATABASE [seven_ORA] SET ANSI_NULL_DEFAULT OFF 
GO
ALTER DATABASE [seven_ORA] SET ANSI_NULLS OFF 
GO
ALTER DATABASE [seven_ORA] SET ANSI_PADDING OFF 
GO
ALTER DATABASE [seven_ORA] SET ANSI_WARNINGS OFF 
GO
ALTER DATABASE [seven_ORA] SET ARITHABORT OFF 
GO
ALTER DATABASE [seven_ORA] SET AUTO_CLOSE OFF 
GO
ALTER DATABASE [seven_ORA] SET AUTO_SHRINK OFF 
GO
ALTER DATABASE [seven_ORA] SET AUTO_UPDATE_STATISTICS ON 
GO
ALTER DATABASE [seven_ORA] SET CURSOR_CLOSE_ON_COMMIT OFF 
GO
ALTER DATABASE [seven_ORA] SET CURSOR_DEFAULT  GLOBAL 
GO
ALTER DATABASE [seven_ORA] SET CONCAT_NULL_YIELDS_NULL OFF 
GO
ALTER DATABASE [seven_ORA] SET NUMERIC_ROUNDABORT OFF 
GO
ALTER DATABASE [seven_ORA] SET QUOTED_IDENTIFIER OFF 
GO
ALTER DATABASE [seven_ORA] SET RECURSIVE_TRIGGERS OFF 
GO
ALTER DATABASE [seven_ORA] SET  DISABLE_BROKER 
GO
ALTER DATABASE [seven_ORA] SET AUTO_UPDATE_STATISTICS_ASYNC OFF 
GO
ALTER DATABASE [seven_ORA] SET DATE_CORRELATION_OPTIMIZATION OFF 
GO
ALTER DATABASE [seven_ORA] SET TRUSTWORTHY OFF 
GO
ALTER DATABASE [seven_ORA] SET ALLOW_SNAPSHOT_ISOLATION OFF 
GO
ALTER DATABASE [seven_ORA] SET PARAMETERIZATION SIMPLE 
GO
ALTER DATABASE [seven_ORA] SET READ_COMMITTED_SNAPSHOT OFF 
GO
ALTER DATABASE [seven_ORA] SET HONOR_BROKER_PRIORITY OFF 
GO
ALTER DATABASE [seven_ORA] SET RECOVERY SIMPLE 
GO
ALTER DATABASE [seven_ORA] SET  MULTI_USER 
GO
ALTER DATABASE [seven_ORA] SET PAGE_VERIFY CHECKSUM  
GO
ALTER DATABASE [seven_ORA] SET DB_CHAINING OFF 
GO
ALTER DATABASE [seven_ORA] SET FILESTREAM( NON_TRANSACTED_ACCESS = OFF ) 
GO
ALTER DATABASE [seven_ORA] SET TARGET_RECOVERY_TIME = 60 SECONDS 
GO
ALTER DATABASE [seven_ORA] SET DELAYED_DURABILITY = DISABLED 
GO
ALTER DATABASE [seven_ORA] SET QUERY_STORE = OFF
GO
USE [seven_ORA]
GO
/****** Object:  User [seven]    Script Date: 9/12/2019 2:30:39 PM ******/
CREATE USER [seven] FOR LOGIN [seven] WITH DEFAULT_SCHEMA=[dbo]
GO
ALTER ROLE [db_owner] ADD MEMBER [seven]
GO
ALTER ROLE [db_accessadmin] ADD MEMBER [seven]
GO
ALTER ROLE [db_securityadmin] ADD MEMBER [seven]
GO
ALTER ROLE [db_ddladmin] ADD MEMBER [seven]
GO
ALTER ROLE [db_backupoperator] ADD MEMBER [seven]
GO
ALTER ROLE [db_datareader] ADD MEMBER [seven]
GO
ALTER ROLE [db_datawriter] ADD MEMBER [seven]
GO

/****** Object:  Table [dbo].[UserInfo]    Script Date: 9/12/2019 2:30:40 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[UserInfo]
(
	[Account] [nvarchar](50) NOT NULL,
	[Address] [nvarchar](50) NULL,
	[AID] [int] NULL,
	[AllowBeta] [int] NULL,
	[AscPassWord] [nvarchar](50) NULL,
	[Birthday] [datetime] NULL,
	[BlockEnd] [datetime] NULL,
	[BlockEnd_Web] [datetime] NULL,
	[BlockStart] [datetime] NULL,
	[BlockStart_Web] [datetime] NULL,
	[City] [nvarchar](50) NULL,
	[Email] [nvarchar](50) NULL,
	[FirstName] [nvarchar](50) NULL,
	[Gen] [nvarchar](50) NULL,
	[Gender] [nvarchar](50) NULL,
	[Jumin] [nvarchar](50) NULL,
	[LastConnect] [nvarchar](50) NULL,
	[LastName] [nvarchar](50) NULL,
	[MailIsConfirm] [bit] NULL,
	[MailOpt] [bit] NULL,
	[MD5PassWord] [nvarchar](50) NULL,
	[MiddleName] [nvarchar](50) NULL,
	[MortherLName] [nvarchar](50) NULL,
	[Nation] [nvarchar](50) NULL,
	[NickName] [nvarchar](50) NULL,
	[RegDate] [datetime] NULL,
	[Right] [int] NULL,
	[States] [nvarchar](50) NULL,
	[Tel] [nvarchar](50) NULL,
	[ZipCode] [nvarchar](50) NULL,
	[realname] [nvarchar](50) NULL,
	[USER_CP] [nvarchar](50) NULL,
	[name] [nvarchar](50) NULL,
	[memberinfo] [int] NOT NULL,
	[Mod] [nvarchar](50) NULL,
	[hint] [nvarchar](50) NULL,
	[answer] [nvarchar](50) NULL,
	[job] [nvarchar](50) NULL,
	[BlockReason] [nvarchar](100) NULL,
	[birthyear] [datetime] NULL,
	[MotherLName] [nvarchar](50) NULL,
	[BlockGM] [varchar](30) NULL,
	CONSTRAINT [PK_UserInfo] PRIMARY KEY CLUSTERED 
(
	[Account] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]
GO
ALTER TABLE [dbo].[UserInfo] ADD  CONSTRAINT [DF_UserInfo_memberinfo]  DEFAULT ((0)) FOR [memberinfo]
GO
