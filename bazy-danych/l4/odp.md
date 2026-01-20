 

## Zadanie 2


```js
use library

db.authors.insertMany([
    {
        "_id": ObjectId("65711ccdcb2d05e2c973fe85"),
        "name": { "first": "J.R.R", "last": "Tolkien" },
        "country": "UK",
        "birth": new Date("Jan 3, 1892"),
        "death": new Date("Sep 2, 1973")
    },
    {
        "_id": ObjectId("65711ccdcb2d05e2c973fe86"),
        "name": { "first": "Jan", "last": "Kochanowski" },
        "country": "Poland",
        "birth": new Date("Jul 5, 1530"),
        "death": new Date("Aug 22, 1584")
    },
    {
        "_id": ObjectId("65711ccdcb2d05e2c973fe87"),
        "name": { "first": "George", "last": "Orwell" },
        "country": "UK",
        "birth": new Date("Jun 25, 1903"),
        "death": new Date("Jan 21, 1950")
    },
    {
        "_id": ObjectId("65711ccdcb2d05e2c973fe88"),
        "name": { "first": "Fiodor", "last": "Dostojewski" },
        "country": "Russia",
        "birth": new Date("Nov 11, 1821"),
        "death": new Date("Feb 9, 1881")
    },
    {
        "_id": ObjectId("65711ccdcb2d05e2c973fe89"),
        "name": { "first": "Franz", "last": "Kafka" },
        "country": "Czechia",
        "birth": new Date("Jul 3, 1883"),
        "death": new Date("Jun 3, 1924")
    },
    {
        "_id": ObjectId("65711ccdcb2d05e2c973fe8a"),
        "name": { "first": "Julian", "last": "Tuwim" },
        "country": "Poland",
        "birth": new Date("Sep 13, 1894"),
        "death": new Date("Dec 27, 1953")
    }
])

db.books.insertMany([
    {
        "_id": ObjectId("65712008cb2d05e2c973fe86"),
        "title": "The Hobbit",
        "isbn": "978-0-261-10295-6",
        "publication_year": 1937,
        "language": "English",
        "author": ObjectId("65711ccdcb2d05e2c973fe85"),
        "publisher": {
        "name": "George Allen & Unwin",
        "country": "UK"
        }
    },
    {
        "_id": ObjectId("65712008cb2d05e2c973fe87"),
        "title": "Odprawa Posłów Greckich",
        "isbn": "978-83-08-05125-7",
        "publication_year": 1578,
        "language": "Polish",
        "author": ObjectId("65711ccdcb2d05e2c973fe86"),
        "publisher": {
        "name": "Drukarnia Łazarzowa",
        "country": "Poland"
        }
    },
    {
        "_id": ObjectId("65712008cb2d05e2c973fe88"),
        "title": "1984",
        "isbn": "978-0-452-28423-4",
        "publication_year": 1949,
        "language": "English",
        "author": ObjectId("65711ccdcb2d05e2c973fe87"),
        "publisher": {
            "name": "Secker & Warburg",
            "country": "UK"
        }
    },
    {
        "_id": ObjectId("65712008cb2d05e2c973fe89"),
        "title": "Zbrodnia i kara",
        "isbn": "978-5-17-079642-1",
        "publication_year": 1866,
        "language": "Russian",
        "author": ObjectId("65711ccdcb2d05e2c973fe88"),
        "publisher": {
            "name": "The Russian Messenger",
            "country": "Russia"
        }
    },
    {
        "_id": ObjectId("65712008cb2d05e2c973fe8a"),
        "title": "Metamorfoza",
        "isbn": "978-80-7207-485-6",
        "publication_year": 1915,
        "language": "German",
        "author": ObjectId("65711ccdcb2d05e2c973fe89"),
        "publisher": {
            "name": "Kurt Wolff Verlag",
            "country": "Germany"
        }
    },
    {
        "_id": ObjectId("65712008cb2d05e2c973fe8b"),
        "title": "Lokomotywa",
        "isbn": "978-83-06-03345-3",
        "publication_year": 1938,
        "language": "Polish",
        "author": ObjectId("65711ccdcb2d05e2c973fe8a"),
        "publisher": {
            "name": "Nasza Księgarnia",
            "country": "Poland"
        }
    },
    {
        "_id": ObjectId("65712008cb2d05e2c973fe8c"),
        "title": "Kwiaty Polskie",
        "isbn": "978-83-06-03346-0",
        "publication_year": 1928,
        "language": "Polish",
        "author": ObjectId("65711ccdcb2d05e2c973fe8a"),
        "publisher": {
            "name": "Gebethner i Wolff",
            "country": "Poland"
        }
    },
    {
        "_id": ObjectId("65712008cb2d05e2c973fe8d"),
        "title": "Bajki",
        "isbn": "978-83-06-03347-7",
        "publication_year": 1946,
        "language": "Polish",
        "author": ObjectId("65711ccdcb2d05e2c973fe8a"),
        "publisher": {
            "name": "Czytelnik",
            "country": "Poland"
        }
    }
])


// Śmieciowe dane (przechodzą)
db.authors.insertOne({
    "_id": ObjectId("000000000000000000000000"),
    "name": { "first": "Jan" },
    "country": "Poland",
    "birth": new Date("Jul 5, 1530"),
    "death": null
})
db.books.insertOne({
    "_id": ObjectId("000000000000000000000001"),
    "title": "Fake Book",
    "isbn": "000-0-00-000000-0",
    "publication_year": 2000,
    "author": ObjectId("000000000000000000000000"),
    "publisher": null
})

// Dodanie schematu z walidacją
db.createCollection("authors", {
  validator: {
    $jsonSchema: {
      bsonType: "object",
      required: ["name", "country", "birth", "death"],
      properties: {
        _id: { bsonType: "objectId" },
        name: { bsonType: "object" ,
                required: ["first", "last"],
                properties: {
                  first: { bsonType: "string" },
                  last: { bsonType: "string" }
                }
              },
        country: { bsonType: "string" },
        birth: { bsonType: "date" },
        death: { bsonType: ["date", "null"] }        
      }
    }
  },
  additionalProperties: false,
  validationLevel: "strict",
  validationAction: "error"
})
```


```js
db.reviews.insertMany([
    {
        "_id": ObjectId("6571300acb2d05e2c973fe90"),
        "book": ObjectId("65712008cb2d05e2c973fe86"),
        "reviewer": {
            "username": "booklover123",
        },
        "rating": 5,
        "review_text": "An amazing journey into Middle-earth. A must-read for fantasy lovers!"
    },
    {
        "_id": ObjectId("6571300acb2d05e2c973fe91"),
        "book": ObjectId("65712008cb2d05e2c973fe88"),
        "reviewer": {
            "username": "booklover123", // Ten sam recenzent
        },
        "rating": 5,
        "review_text": "A chilling depiction of a totalitarian regime. Very thought-provoking."
    },
    {
        "_id": ObjectId("6571300acb2d05e2c973fe92"),
        "book": ObjectId("65712008cb2d05e2c973fe89"),
        "reviewer": {
            "username": "classicReader",
        },
        "rating": 3,
        "review_text": "A profound exploration of morality and redemption. A timeless classic."
    },
    {
        "_id": ObjectId("6571300acb2d05e2c973fe93"),
        "book": ObjectId("65712008cb2d05e2c973fe8a"),
        "reviewer": {
            "username": "metamorphosisFan",
        },
        "rating": 4,
        "review_text": "Kafka's unique style and themes are brilliantly showcased in this novella."
    },
    {
        "_id": ObjectId("6571300acb2d05e2c973fe95"),
        "book": ObjectId("65712008cb2d05e2c973fe8b"),
        "reviewer": {
            "username": "poetryLover",
        },
        "rating": 5,
        "review_text": "Tuwim's poems are delightful and capture the essence of childhood perfectly."
    },
    {
        "_id": ObjectId("6571300acb2d05e2c973fe96"),
        "book": ObjectId("65712008cb2d05e2c973fe8c"),
        "reviewer": {
            "username": "flowerFan"
        },
        "rating": 3,
        "review_text": "Kwiaty Polskie offers beautiful imagery, though some poems feel dated."
    },
    {
        "_id": ObjectId("6571300acb2d05e2c973fe97"),
        "book": ObjectId("65712008cb2d05e2c973fe86"), // Hobbit - druga recenzja
        "reviewer": {
            "username": "hater123",
        },
        "rating": 2, // Ocena poniżej 3
        "review_text": "Too long, too much walking. Boring."
    },
    {
        "_id": ObjectId("6571300acb2d05e2c973fe98"),
        "book": ObjectId("65712008cb2d05e2c973fe86"), // Hobbit - trzecia recenzja
        "reviewer": {
            "username": "fantasyMaster",
        },
        "rating": 4, 
        "review_text": "Solid classic, creates the genre basically."
    }
])

```

Jaki wpływ na wstawianie/wyszukiwanie danyh ma przyjęta
przez Ciebie metoda przechowywania informacji o recenzencie?

Odp: Przechowywanie danych recenzenta jako zagnieżdżonego obiektu (`reviewer`) bezpośrednio w dokumencie recenzji (denormalizacja) upraszcza operację odczytu kompletnej recenzji, ponieważ nie wymaga dodatkowego zapytania (`$lookup`) do kolekcji użytkowników. Wszystkie potrzebne dane do wyświetlenia recenzji są dostępne w jednym dokumencie, co przyspiesza wyszukiwanie.
Jednakże, ma to negatywny wpływ na aktualizację danych: zmiana nazwy użytkownika wymagałaby aktualizacji wszystkich jego recenzji, co jest operacją kosztowną. Wstawianie jest szybkie i proste, ale powoduje redundancję danych (powtarzanie nazwy użytkownika w każdej jego recenzji).


```js


// Dodanie dla każdego dokumentu w kolekcji authors pola "awards" - tablicy nagród {"name": <nazwa>, "year": <rok>}

db.authors.updateMany(
  {},
  {
    $set: {
      awards: []
    }
  }
)

// Dodanie nowego pola "genres" do dokumentów w kolekcji books
// np. "Fantasy", "Classic", "Horror", "Science Fiction"
db.books.updateMany(
    {},
    {
        $set: {
            genres: []
        }
    }
)

// Uaktualnienie dokumentu książki "The Hobbit", aby dodać gatunki "Fantasy" i "Adventure"
db.books.updateOne(
    { title: "The Hobbit" },
    {
        $set: {
            genres: ["Fantasy", "Adventure"]
        }
    }
)

db.books.updateOne(
    { title: "Metamorfoza" },
    {
        $set: {
            genres: ["Classic", "Philosophical Fiction", "Fantasy" ]
        }
    }
)

// Uaktualnienie dokumentu autora "J.R.R. Tolkien", aby dodać nagrodę "International Fantasy Award" z roku 1957
db.authors.updateOne(
    { "name.first": "J.R.R", "name.last": "Tolkien" },
    {
        $push: {
            awards: { name: "International Fantasy Award", year: 1957 }
        }
    }
)



// Uaktualnienie dokumentu autora "Julian Tuwim", aby dodać nagrodę "Order of Polonia Restituta" z roku 1954
db.authors.updateOne(
    { "name.first": "Julian", "name.last": "Tuwim" },
    {
        $push: {
            awards: { name: "Order of Polonia Restituta", year: 1954 }
        }
    }
)

// Dodanie gatunków do książki "Bajki"
db.books.updateOne(
    { title: "Bajki" },
    {
        $set: {
            genres: ["Children", "Fantasy"]
        }
    }
)


```


# Zapytania

```js

// 1. Wszystkie książki napisane po polsku w gatunku "Fantasy"
db.books.find({ genres: "Fantasy", language: "Polish" })
```

## Agregacje

```js
// Książki, których średnia ocena to conajmniej 4
db.books.aggregate([
    {
        $lookup: {
            from: "reviews",
            localField: "_id",
            foreignField: "book",
            as: "book_reviews"
        }
    },
    {
        $unwind: "$book_reviews"
    },
    {
        $group: {
            _id: "$_id",
            title: { $first: "$title" },
            avg_rating: { $avg: "$book_reviews.rating" }
        }
    },
    {
        $match: {
            avg_rating: { $gte: 4 }
        }
    }
])
```

```js
// Wszystkie książki napisane przez autora o konkretnym imieniu i nazwisku
db.books.aggregate([
    {
        $lookup: {
            from: "authors",
            localField: "author",
            foreignField: "_id",
            as: "book_author"
        }
    },
    {
        $unwind: "$book_author"
    },
    {
        $match: {
            "book_author.name.first": "Julian",
            "book_author.name.last": "Tuwim"
        }
    },
    {
        $project: {
            _id: 1,
            title: 1
        }
    }
])
```

```js
// Dane o książkach napisanych przez polskich autorów wraz z nazwiskami tych autorów i średnią oceną ich książek
db.books.aggregate([
    {
        $lookup: {
            from: "authors",
            localField: "author",
            foreignField: "_id",
            as: "book_author"
        }
    },
    {
        $unwind: "$book_author"
    },
    {
        $match: {
            "book_author.country": "Poland"
        }
    },
    {
        $lookup: {
            from: "reviews",
            localField: "_id",
            foreignField: "book",
            as: "book_reviews"
        }
    },
    {
        $unwind: "$book_reviews"
    },
    {
        $group: {
            _id: "$_id",
            title: { $first: "$title" },
            author_name: { $first: { $concat: [ "$book_author.name.first", " ", "$book_author.name.last" ] } },
            avg_rating: { $avg: "$book_reviews.rating" }
        }
    }
])
```

```js
// Wyświetl nazwisko autora oraz liczbę książek, które napisał
db.books.aggregate([
    {
        $group: {
            _id: "$author",
            count: { $sum: 1 }
        }
    },
    {
        $lookup: {
            from: "authors",
            localField: "_id",
            foreignField: "_id",
            as: "author_info"
        }
    },
    {
        $unwind: "$author_info"
    },
    {
        $project: {
            _id: 0,
            lastname: "$author_info.name.last",
            count: 1
        }
    }
])
```

```js
// Policz średnią ocenę książek każdego autora
db.books.aggregate([
    {
        $lookup: {
            from: "reviews",
            localField: "_id",
            foreignField: "book",
            as: "reviews"
        }
    },
    {
        $unwind: "$reviews"
    },
    {
        $group: {
            _id: "$author",
            avg_rating: { $avg: "$reviews.rating" }
        }
    },
    {
        $lookup: {
            from: "authors",
            localField: "_id",
            foreignField: "_id",
            as: "author_info"
        }
    },
    {
        $unwind: "$author_info"
    },
    {
        $project: {
            _id: 0,
            author: { $concat: ["$author_info.name.first", " ", "$author_info.name.last"] },
            avg_rating: 1
        }
    }
])
```

```js
// Znajdź autorów, którzy nie otrzymali żadnej nagrody
db.authors.aggregate([
    {
        $match: {
             $or: [
                { awards: { $exists: false } }, 
                { awards: { $size: 0 } } 
             ]
        }
    },
    {
        $project: {
            name: 1
        }
    }
])
```

```js
// Policz ile książek przypada na każdy gatunek literacki
db.books.aggregate([
    {
        $unwind: "$genres"
    },
    {
        $group: {
            _id: "$genres",
            count: { $sum: 1 }
        }
    }
])
```

```js
// Znajdź osobę, która napisała najwięcej recenzji
db.reviews.aggregate([
    {
        $group: {
            _id: "$reviewer.username",
            count: { $sum: 1 }
        }
    },
    {
        $sort: { count: -1 }
    },
    {
        $limit: 1
    }
])
```

```js
// Policz średnią ocenę książek w zależności od języka
db.books.aggregate([
    {
        $lookup: {
            from: "reviews",
            localField: "_id",
            foreignField: "book",
            as: "reviews"
        }
    },
    {
        $unwind: "$reviews"
    },
    {
        $group: {
            _id: "$language",
            avg_rating: { $avg: "$reviews.rating" }
        }
    }
])
```
