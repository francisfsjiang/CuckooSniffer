#include <iostream>

#include "mime/Mime.h"

char message[] = "\r\n";
char boundary[] = "------------------------566470c33d4e1b5b";

int main() {

    CMimeMessage mail;
    mail.SetBoundary(boundary);
    int nLoadedSize = mail.Load(message, size(message));

// Analyze the message header
    const char* pszField;
    pszField = mail.GetSubject();
    if (pszField != NULL)
        printf("Subject: %s\r\n", pszField);
    pszField = mail.GetFrom();
    if (pszField != NULL)
        printf("From: %s\r\n", pszField);
    pszField = mail.GetFieldValue("X-Priority");
    if (pszField != NULL)
        printf("X-Priority: %s\r\n", pszField);

// Iterate all the descendant body parts
    CMimeBody::CBodyList bodies;
    int nCount = mail.GetBodyPartList(bodies);
    CMimeBody::CBodyList::const_iterator it;
    for (it=bodies.begin(); it!=bodies.end(); it++)
    {
        CMimeBody* pBP = *it;

        // Iterate all the header fields of this body part:
        CMimeHeader::CFieldList& fds = pBP->Fields();
        CMimeHeader::CFieldList::const_iterator itfd;
        for (itfd=fds.begin(); itfd!=fds.end(); itfd++)
        {
            const CMimeField& fd = *itfd;
            printf("%s: %s\r\n", fd.GetName(), fd.GetValue());
        }

        if (pBP->IsText())
        {
            string strText;
            pBP->GetText(strText);
            printf("Content: %s\r\n", strText.c_str());
        }
        else if (pBP->IsAttachment())
        {
            string strName = pBP->GetName();
            printf("File name: %s\r\n", strName.c_str());
            printf("File size: %d\r\n", pBP->GetContentLength());
            strName = "d:\\download\\" + strName;
            pBP->WriteToFile(strName.c_str());
        }
    }
}