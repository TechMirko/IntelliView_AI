/*
per avviare:
    1) apri terminale sotto
    2) digita cd C:\Users\mirko\Desktop\Software\Tecnicamente 3.0\scripts
    3) esegui node server.js
*/
const {writeFile} = require('fs')
const express = require('express')

const app = express()
const path = "C:\\Users\\mirko\\Desktop\\Software\\Tecnicamente 3.0\\test_imgs\\img.jpg"
const port = 8000

app.use(express.raw({type: 'image/jpeg', limit: '5mb'}))

app.post('/intelliView', (req, res) => {
    const image = req.body
    writeFile(path, image, (err) => {
        if (err) {
            console.error(err)
            res.sendStatus(500)
        } else {
            console.log('Immagine salvata')
            res.sendStatus(200)
        }
    })
})

app.listen(port, () => console.log('Server avviato alla porta ' + port))
