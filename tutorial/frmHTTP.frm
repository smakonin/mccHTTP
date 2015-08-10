VERSION 5.00
Begin VB.Form frmHTTP 
   Caption         =   "Form1"
   ClientHeight    =   7860
   ClientLeft      =   60
   ClientTop       =   345
   ClientWidth     =   8805
   LinkTopic       =   "Form1"
   ScaleHeight     =   7860
   ScaleWidth      =   8805
   StartUpPosition =   3  'Windows Default
   Begin VB.CommandButton Command2 
      Caption         =   "Get Site"
      Height          =   435
      Left            =   6480
      TabIndex        =   10
      Top             =   600
      Width           =   2175
   End
   Begin VB.TextBox Text1 
      Height          =   312
      Left            =   1140
      TabIndex        =   5
      Top             =   1080
      Width           =   7575
   End
   Begin VB.TextBox Text2 
      Height          =   6312
      Left            =   60
      MultiLine       =   -1  'True
      ScrollBars      =   2  'Vertical
      TabIndex        =   4
      Top             =   1500
      Width           =   8652
   End
   Begin VB.CommandButton Command1 
      Caption         =   "Get URL"
      Height          =   435
      Left            =   6480
      TabIndex        =   3
      Top             =   60
      Width           =   2175
   End
   Begin VB.TextBox Text3 
      Height          =   312
      Left            =   1140
      TabIndex        =   2
      Top             =   120
      Width           =   1995
   End
   Begin VB.TextBox Text4 
      Alignment       =   1  'Right Justify
      Height          =   312
      Left            =   4440
      TabIndex        =   1
      Top             =   120
      Width           =   1395
   End
   Begin VB.TextBox Text5 
      Height          =   312
      Left            =   1140
      TabIndex        =   0
      Top             =   600
      Width           =   4695
   End
   Begin VB.Label Label1 
      Caption         =   "Proxy Server:"
      Height          =   255
      Left            =   60
      TabIndex        =   9
      Top             =   180
      Width           =   1995
   End
   Begin VB.Label Label2 
      Caption         =   "URL:"
      Height          =   255
      Left            =   60
      TabIndex        =   8
      Top             =   1140
      Width           =   1995
   End
   Begin VB.Label Label3 
      Caption         =   "Proxy Port:"
      Height          =   255
      Left            =   3360
      TabIndex        =   7
      Top             =   180
      Width           =   1995
   End
   Begin VB.Label Label4 
      Caption         =   "Save As/In:"
      Height          =   255
      Left            =   60
      TabIndex        =   6
      Top             =   660
      Width           =   1995
   End
End
Attribute VB_Name = "frmHTTP"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit

Private Sub Command1_Click()
    Dim sError As String
    Dim sUrl As String
    Dim sSaveAs As String
    Dim sPorxyServer As String
    Dim lProxyPort As Long
    
    Screen.MousePointer = vbHourglass
    
    sError = String(256, " ")
    sUrl = CStr(Text1.Text)
    sSaveAs = CStr(Text5.Text)
    sPorxyServer = CStr(Text3.Text)
    lProxyPort = Val(Text4.Text)
    
    Text2.Text = open_url(sError, sUrl, sSaveAs, sPorxyServer, lProxyPort)
    
    If Text2.Text = "" Then
        Text2.Text = "ERROR: " & sError
    End If
        
    Screen.MousePointer = vbDefault
End Sub

Private Sub Command2_Click()
    Dim sError As String
    Dim sUrl As String
    Dim sSaveIn As String
    Dim sPorxyServer As String
    Dim lProxyPort As Long
    
    Screen.MousePointer = vbHourglass
    
    sError = String(256, " ")
    sUrl = CStr(Text1.Text)
    sSaveIn = CStr(Text5.Text)
    sPorxyServer = CStr(Text3.Text)
    lProxyPort = Val(Text4.Text)
    
    Text2.Text = open_website(sError, sUrl, sSaveIn, -1, "", sPorxyServer, lProxyPort)
    
    If Text2.Text = "" Then
        Text2.Text = "ERROR: " & sError
    End If
        
    Screen.MousePointer = vbDefault
End Sub
