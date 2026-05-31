mod utils;
mod rsa;
mod dhsetup;
mod user;
mod ring;

fn print_result(alice_pub_key: u64, bob_pub_key: u64, message: u64, cipher: u64, decrypted: u64) {
    println!("Alice's public key: {}", alice_pub_key);
    println!("Bob's public key: {}", bob_pub_key);
    println!("Message: {}", message);
    println!("Cipher: {}", cipher);
    println!("Decrypted: {}", decrypted);
    println!("Decryption successful: {}", decrypted == message);
}

fn rsa_example() {
    println!("RSA encryption example:\n");

    let alice = rsa::RSA::<10007, 10009>::new();
    let bob = rsa::RSA::<10007, 10009>::new();

    let message: u64 = 12345;
    let cipher = alice.encrypt(message, bob.public_key());
    let decrypted = bob.decrypt(cipher);

    print_result(alice.public_key(), bob.public_key(), message, cipher, decrypted);
}

fn diff_hell() {
    println!("Diffie-Hellman key exchange example:\n");

    let dh = dhsetup::DHSetup::<1234567891>::new();
    let mut alice = user::User::<1234567891>::new(&dh);
    let mut bob = user::User::<1234567891>::new(&dh);

    println!("Generator: {}", dh.get_generator());

    alice.set_key(bob.get_public_key());
    bob.set_key(alice.get_public_key());
    let message: u64 = 12345;
    let cipher = alice.encrypt(message);
    let decrypted = bob.decrypt(cipher);
    print_result(alice.get_public_key(), bob.get_public_key(), message, cipher, decrypted);
}

fn main() {
    rsa_example();
    println!();
    diff_hell();
}